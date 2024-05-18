#include "Scene.h"
#include "Blender.h"
#include "Channel.h"
#include "ConstantBuffer.h"
#include "ConstantBufferEx.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "MathTool.h"
#include "MeshData.h"
#include "NullPixelShader.h"
#include "PixelShader.h"
#include "Rasterizer.h"
#include "Sampler.h"
#include "Stencil.h"
#include "StrManager.h"
#include "Surface.h"
#include "Texture.h"
#include "Topology.h"
#include "TransformCbuf.h"
#include "VS_PS_TFCB.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "imgui.h"
#include <filesystem>
#include <stdexcept>

Scene::Mesh::Mesh(Graphics &gfx, MeshData &mesh_data,
                  const std::string &mesh_name) noexcept
    : m_szName(mesh_name) {
  AddEssentialBind(VertexBuffer::Resolve(gfx, ANSI_TO_UTF8_STR(mesh_name),
                                         *mesh_data.GetVertecies().get()));

  AddEssentialBind(IndexBuffer::Resolve(gfx, ANSI_TO_UTF8_STR(mesh_name),
                                        mesh_data.GetIndicies()));
  AddEssentialBind(
      Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
  {
    Technique shade("shade", Channel::main);
    {
      Step normalDraw("lambertian");
      auto texs = mesh_data.GetTextures();
      if (texs.m_amTex.get()) {
        normalDraw.AddBind(texs.m_amTex);
      }
      if (texs.m_difTex.get()) {
        normalDraw.AddBind(texs.m_difTex);
      }
      if (texs.m_specTex.get()) {
        normalDraw.AddBind(texs.m_specTex);
      }
      if (texs.m_normTex.get()) {
        normalDraw.AddBind(texs.m_normTex);
      }
      normalDraw.AddBind(
          Rasterizer::Resolve(gfx, mesh_data.GetTextures().m_hasAlpha));
      normalDraw.AddBind(
          Blender::Resolve(gfx, mesh_data.GetTextures().m_hasAlpha));
      auto pvs = VertexShader::Resolve(gfx, mesh_data.GetVSPath());
      normalDraw.AddBind(InputLayout::Resolve(
          gfx, mesh_data.GetVertecies()->GetLayout(), *pvs));
      normalDraw.AddBind(std::move(pvs));
      normalDraw.AddBind(PixelShader::Resolve(gfx, mesh_data.GetPSPath()));
      normalDraw.AddBind(Sampler::Resolve(gfx));
      normalDraw.AddBind(std::make_shared<CachingPixelConstantBuffer>(
          gfx, mesh_data.GetConstData(), 2u));
      normalDraw.AddBind(std::make_shared<TransformCbuf>(gfx, *this));
      shade.AddStep(normalDraw);
    }
    AddTechnique(shade);
  }
  {
    Technique outline("OutLine", Channel::main);
    {
      {
        Step mask("outlineMask");
        mask.AddBind(InputLayout::Resolve(
            gfx, mesh_data.GetVertecies()->GetLayout(),
            *VertexShader::Resolve(gfx, L"res\\cso\\Solid_VS.cso")));
        mask.AddBind(std::make_shared<TransformCbuf>(gfx, *this));
        outline.AddStep(mask);
      }
      {
        Step draw_to_RenderTarget("outlineDraw");

        DCBuf::RawLayout cBufferLayout;
        cBufferLayout.Add<DCBuf::Float4>("outline_color");
        DCBuf::Buffer buffer(std::move(cBufferLayout));
        buffer["outline_color"] = DirectX::XMFLOAT4{1.0f, 0.4f, 0.4f, 1.0f};

        draw_to_RenderTarget.AddBind(
            std::make_shared<CachingPixelConstantBuffer>(gfx, buffer, 3u));
        draw_to_RenderTarget.AddBind(InputLayout::Resolve(
            gfx, mesh_data.GetVertecies()->GetLayout(),
            *VertexShader::Resolve(gfx, L"res\\cso\\Solid_VS.cso")));
        draw_to_RenderTarget.AddBind(
            std::make_shared<TransformCbuf>(gfx, *this));
        outline.AddStep(draw_to_RenderTarget);
      }
    }
    outline.SetActiveState(false);
    AddTechnique(outline);
  }
  {
    Technique map{"ShadowMap", Channel::shadow};
    {
      Step draw("shadowMapping");

      // TODO: better sub-layout generation tech for future consideration maybe
      draw.AddBind(InputLayout::Resolve(
          gfx, mesh_data.GetVertecies()->GetLayout(),
          *VertexShader::Resolve(gfx, L"res\\cso\\Solid_VS.cso")));

      draw.AddBind(std::make_shared<TransformCbuf>(gfx, *this));

      // TODO: might need to specify rasterizer when doubled-sided models start
      // being used

      map.AddStep(draw);
    }
    AddTechnique(map);
  }
}
void Scene::Mesh::Submit(DirectX::FXMMATRIX accumulateTransform,
                         size_t channel) const noexcept(!IS_DEBUG) {
  DirectX::XMStoreFloat4x4(&m_transform, accumulateTransform);
  Drawable::Submit(channel);
}

std::string Scene::Mesh::GetName() noexcept { return m_szName; }

DirectX::XMMATRIX Scene::Mesh::GetTransformXM() const noexcept {
  return DirectX::XMLoadFloat4x4(&m_transform);
}

void Scene::Mesh::SetTransform(DirectX::XMMATRIX transform) noexcept {
  DirectX::XMStoreFloat4x4(&m_transform, transform);
}

int Scene::Node::GetId() const noexcept { return m_id; }

const std::string &Scene::Node::GetName() const noexcept {
  return m_szNodeName;
}

bool Scene::Node::HasChild() const noexcept { return m_pChilds.empty(); }

Scene::TransformParams &Scene::Node::GetAppliedTransform() noexcept {
  return m_AppliedTransform;
}

void Scene::Node::Submit(DirectX::FXMMATRIX accumulateTransform,
                         size_t channel) const noexcept {
  DirectX::XMMATRIX appliedTransform =
      DirectX::XMMatrixRotationRollPitchYaw(m_AppliedTransform.rot.x,
                                            m_AppliedTransform.rot.y,
                                            m_AppliedTransform.rot.z) *
      DirectX::XMMatrixScaling(m_AppliedTransform.scale.x,
                               m_AppliedTransform.scale.y,
                               m_AppliedTransform.scale.z) *
      DirectX::XMMatrixTranslation(m_AppliedTransform.pos.x,
                                   m_AppliedTransform.pos.y,
                                   m_AppliedTransform.pos.z);
  const auto build = appliedTransform *
                     DirectX::XMLoadFloat4x4(&m_BaseTransform) *
                     accumulateTransform;
  for (const auto pm : m_pMeshes) {
    pm->Submit(build, channel);
  }
  for (const auto &pc : m_pChilds) {
    pc->Submit(build, channel);
  }
}

void Scene::Node::SetSelectStatus(bool status) noexcept {
  m_selected = status;
  for (auto &i : m_pChilds) {
    i->SetSelectStatus(status);
  }
}

Scene::Node::Node(int id, const std::wstring &NodeName,
                  std::vector<Mesh *> pMeshes,
                  const DirectX::XMMATRIX &transform)
    : m_id(id), m_pMeshes(std::move(pMeshes)),
      m_szNodeName(UTF8_TO_ANSI_STR(NodeName)) {
  DirectX::XMStoreFloat4x4(&m_BaseTransform, transform);
  DirectX::XMFLOAT4X4 id_mtx;
  DirectX::XMStoreFloat4x4(&id_mtx, DirectX::XMMatrixIdentity());
  m_AppliedTransform.pos = math_tool::ExtraTranslation(id_mtx);
  m_AppliedTransform.rot = math_tool::ExtraEulerAngle(id_mtx);
  m_AppliedTransform.scale = math_tool::ExtraScaling(id_mtx);
}

bool Scene::Node::ParentSelected() const noexcept {
  Node *cur_node = m_parent;
  if (cur_node == nullptr) {
    return false;
  }
  if (cur_node->m_selected) {
    return true;
  } else {
    return cur_node->ParentSelected();
  }
}

void Scene::Node::AddChild(std::unique_ptr<Node> child) noexcept(!IS_DEBUG) {
  assert(child);
  m_pChilds.emplace_back(std::move(child));
}

void Scene::Node::SetAccumulateTransform(DirectX::XMMATRIX accu_tf) noexcept(
    !IS_DEBUG) {
  DirectX::XMMATRIX appliedTransform =
      DirectX::XMMatrixRotationRollPitchYaw(m_AppliedTransform.rot.x,
                                            m_AppliedTransform.rot.y,
                                            m_AppliedTransform.rot.z) *
      DirectX::XMMatrixScaling(m_AppliedTransform.scale.x,
                               m_AppliedTransform.scale.y,
                               m_AppliedTransform.scale.z) *
      DirectX::XMMatrixTranslation(m_AppliedTransform.pos.x,
                                   m_AppliedTransform.pos.y,
                                   m_AppliedTransform.pos.z);
  const auto build =
      appliedTransform * DirectX::XMLoadFloat4x4(&m_BaseTransform) * accu_tf;
  for (const auto &pm : m_pMeshes) {
    pm->SetTransform(build);
  }
  for (const auto &pc : m_pChilds) {
    pc->SetAccumulateTransform(build);
  }
}

void Scene::Node::Accept(TNodeProbe &probe) noexcept(!IS_DEBUG) {
  if (probe.VisitNode(*this)) {
    DirectX::XMMATRIX appliedTransform =
        DirectX::XMMatrixRotationRollPitchYaw(m_AppliedTransform.rot.x,
                                              m_AppliedTransform.rot.y,
                                              m_AppliedTransform.rot.z) *
        DirectX::XMMatrixScaling(m_AppliedTransform.scale.x,
                                 m_AppliedTransform.scale.y,
                                 m_AppliedTransform.scale.z) *
        DirectX::XMMatrixTranslation(m_AppliedTransform.pos.x,
                                     m_AppliedTransform.pos.y,
                                     m_AppliedTransform.pos.z);
    const auto build =
        appliedTransform * DirectX::XMLoadFloat4x4(&m_BaseTransform);
    SetAccumulateTransform(build);
  }
  for (auto &i : m_pChilds) {
    i->m_parent = this;
    i->Accept(probe);
  }
}

void Scene::Node::Accept(MaterialProbe &probe) noexcept(!IS_DEBUG) {
  probe.SetSelectStatus(m_selected);
  for (auto &i : m_pMeshes) {
    probe.SetCurMeshName(i->GetName());
    if (probe.IsSelected()) {
      ImGui::TextColored({0.8f, 0.8f, 0.8f, 1.0f}, i->GetName().c_str());
    }
    i->Accept(probe);
  }
}

void Scene::Node::AcceptToShowTree(NodeProbe &probe) noexcept(!IS_DEBUG) {
  if (probe.PushNode(*this)) {
    for (auto &i : m_pChilds) {
      i->AcceptToShowTree(probe);
    }
    probe.PopNode();
  }
}

Scene::Model::Model(Graphics &gfx, ModelSetting &option)
    : m_szModelName(option.szModelName) {
  Assimp::Importer importer;
  const aiScene *pScene = importer.ReadFile(
      option.szModelPath,
      aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
          aiProcess_ConvertToLeftHanded | aiProcess_GenNormals |
          aiProcess_CalcTangentSpace);
  if (pScene == nullptr) {
    throw std::logic_error(importer.GetErrorString());
  }

  for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
    m_pMeshes.emplace_back(
        ParseMesh(gfx, *pScene->mMeshes[i], option, pScene->mMaterials));
  }
  int next_id = 0;
  m_pRoot = ParseNode(next_id, *pScene->mRootNode);
}

std::unique_ptr<Scene::Mesh>
Scene::Model::ParseMesh(Graphics &gfx, const aiMesh &mesh, ModelSetting &option,
                        const aiMaterial *const *pMaterial) {
  MeshData mesh_data =
      MeshData(gfx, option.szModelPath, option.szModelName, mesh, pMaterial);
  return std::make_unique<Mesh>(gfx, mesh_data,
                                option.szModelName + "#" +
                                    std::string(mesh.mName.C_Str()));
}

const std::string &Scene::Model::GetName() const noexcept {
  return m_szModelName;
}

std::unique_ptr<Scene::Node> Scene::Model::ParseNode(int &next_id,
                                                     const aiNode &node) {
  const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
      reinterpret_cast<const DirectX::XMFLOAT4X4 *>(&node.mTransformation)));
  std::vector<Mesh *> curMeshPtrs;
  curMeshPtrs.reserve(node.mNumMeshes);
  for (UINT i = 0; i < node.mNumMeshes; i++) {
    UINT meshIdx = node.mMeshes[i];
    curMeshPtrs.emplace_back(m_pMeshes.at(meshIdx).get());
  }
  std::unique_ptr<Node> pNode = std::make_unique<Node>(
      next_id, ANSI_TO_UTF8_STR(std::string(node.mName.C_Str())),
      std::move(curMeshPtrs), transform);
  for (UINT i = 0; i < node.mNumChildren; i++) {
    pNode->AddChild(ParseNode(++next_id, *node.mChildren[i]));
  }
  return pNode;
}

void Scene::Model::AcceptToShowTree(NodeProbe &probe) noexcept(!IS_DEBUG) {
  m_pRoot->AcceptToShowTree(node_probe);
}

void Scene::Model::Accept(TNodeProbe &probe) noexcept(!IS_DEBUG) {
  m_pRoot->Accept(node_probe);
}

void Scene::Model::Submit(size_t channel) const noexcept(!IS_DEBUG) {
  m_pRoot->Submit(DirectX::XMMatrixIdentity() *
                      DirectX::XMMatrixScaling(m_scale, m_scale, m_scale) *
                      DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z),
                  channel);
}

void Scene::Model::LinkTechniques(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  for (auto &i : m_pMeshes) {
    i->LinkTechniques(rg);
  }
}

void Scene::Model::ApplyTransformation() noexcept(!IS_DEBUG) {
  m_pRoot->SetAccumulateTransform(
      DirectX::XMMatrixIdentity() *
      DirectX::XMMatrixScaling(m_scale, m_scale, m_scale) *
      DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z));
}

void Scene::Model::Scale(float scale) noexcept { m_scale = scale; }
void Scene::Model::SetPos(float x, float y, float z) noexcept {
  m_pos = {x, y, z};
}
void Scene::Model::SetPos(DirectX::XMFLOAT3 pos) noexcept { m_pos = pos; }

void Scene::Scene::AddModel(Graphics &gfx,
                            ModelSetting &setting) noexcept(!IS_DEBUG) {
  m_models.emplace_back(std::make_unique<Model>(gfx, setting));
  signalModelAdded = true;
}

void Scene::Scene::LinkAddedModel(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  m_models.back()->LinkTechniques(rg);
}

void Scene::Scene::Submit(size_t channel) const noexcept(!IS_DEBUG) {
  for (auto &i : m_models)
    i->Submit(channel);
}

void Scene::Scene::LinkTechniques(Rgph::RenderGraph &rg) noexcept(!IS_DEBUG) {
  for (auto &i : m_models)
    i->LinkTechniques(rg);
}

std::vector<std::unique_ptr<Scene::Model>> &Scene::Scene::GetModels() noexcept {
  return m_models;
}
