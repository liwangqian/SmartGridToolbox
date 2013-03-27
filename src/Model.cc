#include "Model.h"
#include "Component.h"

namespace SmartGridToolbox 
{
   Model::~Model() 
   {
   }

   void Model::AddComponent(Component & comp)
   {
      std::pair<ComponentMap::iterator, bool> result = 
         compMap_.insert(make_pair(comp.GetName(), &comp));
      if (result.second == 0) {
         throw "ERROR: Component already exists in model!";
      }
      else
      {
         compVec_.push_back(&comp);
      }
   }
}
