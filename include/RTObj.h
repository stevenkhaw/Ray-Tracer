/**************************************************
Obj is subclass class of Geometry
that loads an obj file.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include "RTGeometry.h"
#ifndef __OBJ_H__
#define __OBJ_H__

class Obj : public RTGeometry {
public:

    void init(const char* filename);

};

#endif 
