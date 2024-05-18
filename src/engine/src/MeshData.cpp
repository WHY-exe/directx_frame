#include "MeshData.h"
#include "DynamicConstantBuffer.h"
#include "StrManager.h"
#include "Texture.h"
#include <filesystem>
MeshData::MeshData(Graphics &gfx, const std::string &szModelPath,
                   const std::string &szModelName, const aiMesh &mesh,
                   const aiMaterial *const *pMaterial)
    : m_szModelPath(szModelPath), m_szModelName(szModelName) {
  SetVertecies(mesh);
  SetIndicies(mesh);
  SetMaterial(gfx, mesh, pMaterial);
}
void MeshData::SetModelName(const std::string &szModelName) noexcept {
  m_szModelName = szModelName;
}
TextureInfo &MeshData::GetTextures() noexcept { return m_texInfo; }
std::unique_ptr<Vertex::DataBuffer> &MeshData::GetVertecies() noexcept {
  return m_pVertexData;
}
std::vector<UINT> &MeshData::GetIndicies() noexcept { return m_indicies; }

DCBuf::Buffer &MeshData::GetConstData() noexcept { return m_DynamicConstData; }

const std::wstring &MeshData::GetPSPath() const noexcept { return m_szPSPath; }

const std::wstring &MeshData::GetVSPath() const noexcept { return m_szVSPath; }

void MeshData::SetVertecies(const aiMesh &mesh) {
  Vertex::Layout vlayout = Vertex::Layout();
  if (mesh.HasPositions()) {
    vlayout.Append(Vertex::Position3D);
  }
  if (mesh.HasNormals()) {
    vlayout.Append(Vertex::Normal).Append(Vertex::Tangent);
  }
  if (mesh.HasTextureCoords(0)) {
    vlayout.Append(Vertex::Tex2D);
  }
  m_pVertexData = std::make_unique<Vertex::DataBuffer>(vlayout);
  for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
    for (size_t i_ele = 0; i_ele < vlayout.Count(); i_ele++) {
      switch (vlayout.ResolveByIndex(i_ele).GetType()) {
      case Vertex::Position3D:
        m_pVertexData->EmplaceBack(
            *reinterpret_cast<DirectX::XMFLOAT3 *>(&mesh.mVertices[i]));
        break;
      case Vertex::Normal:
        m_pVertexData->EmplaceBack(
            *reinterpret_cast<DirectX::XMFLOAT3 *>(&mesh.mNormals[i]));
        break;
      case Vertex::Tex2D:
        m_pVertexData->EmplaceBack(
            *reinterpret_cast<DirectX::XMFLOAT2 *>(&mesh.mTextureCoords[0][i]));
        break;
      case Vertex::Tangent:
        m_pVertexData->EmplaceBack(
            *reinterpret_cast<DirectX::XMFLOAT3 *>(&mesh.mTangents[i]));
        break;
      case Vertex::Bitangent:
        m_pVertexData->EmplaceBack(
            *reinterpret_cast<DirectX::XMFLOAT3 *>(&mesh.mBitangents[i]));
        break;
      default:
        assert("Bad Element Type" && false);
        break;
      }
    }
  }
}

void MeshData::SetIndicies(const aiMesh &mesh) {
  m_indicies.reserve((size_t)mesh.mNumFaces * 3);
  for (unsigned int f = 0; f < mesh.mNumFaces; f++) {
    const aiFace &face = mesh.mFaces[f];
    assert(face.mNumIndices == 3);
    for (unsigned int i = 0; i < 3; i++) {
      m_indicies.push_back(face.mIndices[i]);
    }
  }
}

void MeshData::SetMaterial(Graphics &gfx, const aiMesh &mesh,
                           const aiMaterial *const *pMaterial) {
  const std::filesystem::path &ModelPath = m_szModelPath;
  std::string szTexRootPath = ModelPath.parent_path().string();
  float shininess = 30.0f;
  DirectX::XMFLOAT4 specColor = {0.18f, 0.18f, 0.18f, 1.0f};
  DirectX::XMFLOAT4 diffuseColor = {0.45f, 0.45f, 0.85f, 1.0f};
  DirectX::XMFLOAT4 ambientColor = {0.45f, 0.45f, 0.45f, 1.0f};
  if (mesh.mMaterialIndex >= 0) {
    using namespace std::string_literals;
    DCBuf::RawLayout cBufferLayout;
    cBufferLayout.Add<DCBuf::Float4>("Ambient");
    cBufferLayout.Add<DCBuf::Bool>("enNormal");
    cBufferLayout.Add<DCBuf::Float>("normalMapWeight");
    cBufferLayout.Add<DCBuf::Bool>("enSpec");
    cBufferLayout.Add<DCBuf::Float4>("SpecColor");
    cBufferLayout.Add<DCBuf::Float>("SpecIntensity");
    cBufferLayout.Add<DCBuf::Float>("SpecPow");
    cBufferLayout.Add<DCBuf::Bool>("hasAmbient");
    cBufferLayout.Add<DCBuf::Bool>("hasGloss");

    m_DynamicConstData = DCBuf::Buffer(std::move(cBufferLayout));

    bool hasAlphaGloss = false;
    m_szPSPath = L"res\\cso\\PS";
    m_szVSPath = L"res\\cso\\VSTex";

    auto &material = *pMaterial[mesh.mMaterialIndex];
    aiString texPath;
    if (material.GetTexture(aiTextureType_AMBIENT, 0, &texPath) ==
            aiReturn_SUCCESS &&
        texPath.length != 0) {
      m_texInfo.m_amTex = Texture::Resolve(
          gfx, ANSI_TO_UTF8_STR(szTexRootPath + "\\"s + texPath.C_Str()), 3);
      if (m_texInfo.m_amTex->HasAlpha() && !m_texInfo.m_hasAlpha) {
        m_texInfo.m_hasAlpha = true;
      }
    } else {
      material.Get(AI_MATKEY_COLOR_AMBIENT,
                   reinterpret_cast<aiColor3D &>(ambientColor));
    }
    if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texPath) ==
            aiReturn_SUCCESS &&
        texPath.length != 0) {
      m_texInfo.m_difTex = Texture::Resolve(
          gfx, ANSI_TO_UTF8_STR(szTexRootPath + "\\"s + texPath.C_Str()));
      if (m_texInfo.m_difTex->HasAlpha() && !m_texInfo.m_hasAlpha) {
        m_texInfo.m_hasAlpha = true;
      }
    } else {
      material.Get(AI_MATKEY_COLOR_DIFFUSE,
                   reinterpret_cast<aiColor3D &>(diffuseColor));
    }
    if (material.GetTexture(aiTextureType_SPECULAR, 0, &texPath) ==
            aiReturn_SUCCESS &&
        texPath.length != 0) {
      m_texInfo.m_specTex = Texture::Resolve(
          gfx, ANSI_TO_UTF8_STR(szTexRootPath + "\\"s + texPath.C_Str()), 1);
      hasAlphaGloss = m_texInfo.m_specTex->HasAlpha();
    }
    if (!hasAlphaGloss) {
      material.Get(AI_MATKEY_COLOR_SPECULAR,
                   reinterpret_cast<aiColor3D &>(specColor));
    }
    if (material.GetTexture(aiTextureType_NORMALS, 0, &texPath) ==
            aiReturn_SUCCESS ||
        material.GetTexture(aiTextureType_DISPLACEMENT, 0, &texPath) ==
                aiReturn_SUCCESS &&
            texPath.length != 0) {
      m_texInfo.m_normTex = Texture::Resolve(
          gfx, ANSI_TO_UTF8_STR(szTexRootPath + "\\"s + texPath.C_Str()), 2);
    } else {
      material.Get(AI_MATKEY_SHININESS, shininess);
    }
    m_szPSPath += std::wstring(m_texInfo.m_difTex.get() ? L"Tex" : L"") +
                  (m_texInfo.m_specTex.get() ? L"Spec" : L"") +
                  (m_texInfo.m_normTex.get() ? L"Norm" : L"") +
                  (m_texInfo.m_hasAlpha ? L"Alpha" : L"") + L".cso";
    m_szVSPath +=
        std::wstring(m_texInfo.m_normTex.get() ? L"Norm" : L"") + L".cso";

    m_DynamicConstData["Ambient"] = ambientColor;
    m_DynamicConstData["SpecColor"] = specColor;
    m_DynamicConstData["SpecIntensity"] = 0.5f;
    m_DynamicConstData["SpecPow"] = shininess;
    m_DynamicConstData["hasAmbient"] = m_texInfo.m_amTex.get() ? true : false;
    m_DynamicConstData["hasGloss"] = hasAlphaGloss;
    m_DynamicConstData["enNormal"] = true;
    m_DynamicConstData["normalMapWeight"] = 1.0f;
  }
}
