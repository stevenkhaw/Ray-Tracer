/**************************************************
RTScene.cpp contains the implementation of the draw command
*****************************************************/
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"

// The scene init definition 
#include "RTScene.inl"


using namespace glm;
void RTScene::buildTriangleSoup() {
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera->computeMatrices();
    shader->view = camera->view;
    shader->projection = camera->proj;
    shader->nlights = light.size();
    shader->lightpositions.resize(shader->nlights);
    shader->lightcolors.resize(shader->nlights);
    int count = 0;
    for (std::pair<std::string, Light*> entry : light) {
        shader->lightpositions[count] = (entry.second)->position;
        shader->lightcolors[count] = (entry.second)->color;
        count++;
    }

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    Node* cur = node["world"]; // root of the tree
    mat4 cur_VM = camera->view;

    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);

    // Compute total number of connectivities in the graph; this would be an upper bound for
    // the stack size in the depth first search over the directed acyclic graph
    int total_number_of_edges = 0;
    for (const auto& n : node) total_number_of_edges += n.second->childnodes.size();

    // If you want to print some statistics of your scene graph  -- sure why not
    std::cout << "total numb of nodes = " << node.size() << std::endl;
    std::cout << "total number of edges = " << total_number_of_edges << std::endl;

    while (!dfs_stack.empty()) {

        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the number of edges in the graph.
        if (dfs_stack.size() > total_number_of_edges) {
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }

        // top-pop the stacks
        cur = dfs_stack.top();  dfs_stack.pop();
        cur_VM = matrix_stack.top(); matrix_stack.pop();
        
        // draw all the models at the current node

        for (int i = 0; i < cur->models.size(); i++) {
            mat4 modelview = cur_VM * (cur->modeltransforms[i]);

            for (int j = 0; j < cur->models[i]->geometry->count; j++) {
                Triangle *currTriangle = cur->models[i]->geometry->elements[j];

                for (int k = 0; k < 3; k++) {
                                        //      cast as vec4, transform                  homogenize
                    currTriangle->P[k] = (modelview * vec4(currTriangle->P[k])).xyz / (modelview * vec4(currTriangle->P[k])).w;
                    currTriangle->N[k] = normalize(transpose(inverse(modelview)) * currTriangle->N[k]);
                }

                currTriangle->material = (cur->models[i])->material;

                this->triangle_soup.push_back(currTriangle);
            }
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (size_t i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            matrix_stack.push(cur_VM * (cur->childtransforms[i]));
        }
    }
}


