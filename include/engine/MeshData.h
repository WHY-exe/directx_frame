#pragma once
#include "DynamicConstantBUffer.h"
#include "Graphics.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

struct TextureInfo {
  std::shared_ptr<Texture> m_amTex;
  std::shared_ptr<Texture> m_difTex;
  std::shared_ptr<Texture> m_specTex;
  std::shared_ptr<Texture> m_normTex;
  bool m_hasAlpha = false;
};

class MeshData {
public:
  MeshData(Graphics &gfx, const std::string &szModelPath,
           const std::string &szModelName, const aiMesh &mesh,
           const aiMaterial *const *pMaterial);
  void SetModelName(const std::string &szModelName) noexcept;
  TextureInfo &GetTextures() noexcept;
  std::unique_ptr<Vertex::DataBuffer> &GetVertecies() noexcept;
  std::vector<UINT> &GetIndicies() noexcept;
  DCBuf::Buffer &GetConstData() noexcept;
  const std::wstring &GetPSPath() const noexcept;
  const std::wstring &GetVSPath() const noexcept;

private:
  void SetVertecies(const aiMesh &mesh);
  void SetIndicies(const aiMesh &mesh);
  void SetMaterial(Graphics &gfx, const aiMesh &mesh,
                   const aiMaterial *const *pMaterial);

private:
  const std::string m_szModelPath;
  std::wstring m_szPSPath;
  std::wstring m_szVSPath;
  std::string m_szModelName;
  std::unique_ptr<Vertex::DataBuffer> m_pVertexData;
  std::vector<UINT> m_indicies;
  DCBuf::Buffer m_DynamicConstData;
  TextureInfo m_texInfo;
};