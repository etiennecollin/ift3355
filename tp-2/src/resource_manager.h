#pragma once
#include "object.h"

class ResourceManager {
  public:

  // Initialise l'instance
  static ResourceManager* Instance();

  // Relâche l'instance
  static void Release();

  // Tous les différents matériaux sont conversés ici question de performance
  std::map<std::string, Material> materials;
private:
  static ResourceManager* Instance_;
 
  ResourceManager();

  ~ResourceManager();
};