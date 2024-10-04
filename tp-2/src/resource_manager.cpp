#include "resource_manager.h"

ResourceManager *ResourceManager::Instance_ = NULL;

ResourceManager::ResourceManager(){};

ResourceManager::~ResourceManager() {
  materials.clear();
};

ResourceManager* ResourceManager::Instance() {
  if (Instance_ == NULL) {
    Instance_ = new ResourceManager();
  }

  return Instance_;
}

void ResourceManager::Release() {
  delete Instance_;
  Instance_ = nullptr;
}