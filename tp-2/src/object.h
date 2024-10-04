#pragma once

#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <cfloat>
#include <string>

#include "basic.h"
#include "bitmap_image/bitmap_image.h"
#include "linalg/linalg.h"
using namespace linalg::aliases;
#include "aabb.h"

// Le type d'une "liste de paramètres", e.g. une map de strings vers des listes de nombres.
typedef std::map<std::string, std::vector<double> > ParamList;

// Une classe pour encapsuler tous les paramètres d'un matériau.
class Material
{
public:
    // Constructeurs
    Material() {};
    Material(bitmap_image &b, ParamList &params) { init(b, params); }

    void init(bitmap_image &b, ParamList &params)
    {
#define SET_BITMAP(_name) _name = b;
        SET_BITMAP(texture_albedo);
        
#define SET_VEC3(_name) _name = params[#_name].size() == 3 ? double3{params[#_name][0],params[#_name][1],params[#_name][2]} : double3{0,0,0};
        SET_VEC3(color_albedo);

#define SET_FLOAT(_name) _name = params[#_name].size() == 1 ? params[#_name][0] : 0;
        SET_FLOAT(k_ambient)
        SET_FLOAT(k_diffuse)
        SET_FLOAT(k_specular)
        SET_FLOAT(metallic)

        SET_FLOAT(shininess)
        SET_FLOAT(refractive_index)

        SET_FLOAT(k_reflection)
        SET_FLOAT(k_refraction)
    }
    // Texture du matériel NON-normalisé [r,g,b \in 0..=255]
    bitmap_image texture_albedo;

    // Couleur du matériel normalisé [r,g,b \in 0..=1] si aucune texture n'est présent
    double3 color_albedo;

    // Coefficient qui module les paramètres de la lumière ambiente, difuse et spéculaire
    double k_ambient;
    double k_diffuse;
    double k_specular;

    // Coefficient métallique pour la réflexion spéculaire [0 -> Surface métallique, 1 -> Surface plastique]
    double metallic;

    // Coefficent de "brillance" (Exposant Spéculaire).
    double shininess;

    // Indice de réfraction du matériel [1 correspond à l'air ambiant]
    double refractive_index;

    // Coefficient de réflexion de la couleur capturée lors du lancer de rayon.
    double k_reflection;

    // Coefficient de réfraction de la couleur capturée lors du lancer de rayon.
    double k_refraction;
};

// Une classe pour encapsuler l'information suite à l'intersection.
class Intersection {
public:
	// La profondeur du rayon
	double depth;

	// La position de l'intersection
	double3 position;

	// La normale à la surface d'intersection
	double3 normal;

	// Les coordonnées UV associées à l'intersection [entre 0 et 1]
	double2 uv;

    // La clé associée au matériel utilisé.
    std::string key_material;

	Intersection() : depth(DBL_MAX) {}
};

// Classe abstraite de base pour les objets.
class Object
{
public:
    double4x4 transform;   // Transformation de l'espace de l'objet à l'espace global (local --> global).
    double4x4 i_transform; // Transformation de l'espace de global à l'espace de l'objet (global --> local).
    
    double3x3 n_transform; // Transformation de l'espace de l'objet à l'espace global pour les normales (local --> global).

    std::string key_material; // Matériau de l'objet.

    // Mets en place les 3 transformations à partir de la transformation (global-vers-objet) donnée.
    void setup_transform(double4x4 m)
    {
        transform = m;
        i_transform = inverse(m);
        n_transform = {{i_transform[0][0],i_transform[1][0],i_transform[2][0]},
                       {i_transform[0][1],i_transform[1][1],i_transform[2][1]},
                       {i_transform[0][2],i_transform[1][2],i_transform[2][2]}};
    };

    // Intersecte l'objet avec le rayon donné dans le repère global.
    // Retourne true s'il y a eu une intersection avec de l'information sur l'intersection.
    bool intersect(Ray ray, 
                   double t_min, double t_max, 
                   Intersection* hit) {

        //Rayon dans le repère locale
        Ray lray{mul(i_transform, {ray.origin,1}).xyz(), mul(i_transform, {ray.direction,0}).xyz()};
        
        //!!! NOTE UTILE : Pour calculer la profondeur dans local_intersect(), si l'intersection se passe à
        //                 ray.origin + ray.direction * t, alors t est la PROFONDEUR
        
        //!!! NOTE UTILE : Assurez-vous que la profondeur du rayon soit contenu entre t_min et t_max.
        if (local_intersect(lray, t_min, t_max, hit)) 
        {
            //!!! NOTE UTILE : Assurez-vous que la normale est bien normalisée
            //                 et que les coordonnées UV sont contenus [0..1]

            hit->key_material = key_material;

            // Transforme les coordonnées de l'intersection dans le repère GLOBAL.
            hit->position = mul(transform,{hit->position,1}).xyz();
            hit->normal = normalize(mul(n_transform, hit->normal));
            
            return true;
        }

        return false;
    };

    // Construit la boite englobante pour l'objet donnée.
    //
    // !!!NOTE UTILE : Ceci doit être appelé après que les objets soient formées et avant 
    //                 que le lancer de rayons ne commence. 
    // !!!NOTE UTILE : Celui-ci doit se faire dans le repère GLOBAL!
    virtual AABB compute_aabb() {
        AABB aabb;
        aabb.min = double3{-DBL_MAX, -DBL_MAX, -DBL_MAX};
	    aabb.max = double3{DBL_MAX, DBL_MAX, DBL_MAX};

        return aabb;
    };
    
protected:
    // Intersecte l'objet avec le rayon donné dans le repère local.
    // Cette fonction est spécifique à chaque sous-type d'objet.
    // Retourne true s'il y a eu une intersection, hit est alors mis à jour avec les paramètres.
    virtual bool local_intersect(Ray ray, double t_min, double t_max, Intersection* hit) = 0;
};


// Espace Local: Sphère centrée à l'origine avec un rayon (radius).
class Sphere : public Object
{
public:
    //Rayon de la sphère
    double radius;

    Sphere(double r) : radius(r) {};

    //À adapter pour la sphère.
    virtual AABB compute_aabb();
protected:
    //À adapter pour la sphère
    virtual bool local_intersect(Ray ray, double t_min, double t_max, Intersection* hit);
};


// Espace Local: Quad(Rectangle) centrée à l'origine tel que la normale
//               est Z+ pour une largeur de (2 * half_size) x (2 * half_size)
class Quad : public Object
{
public:
    //Demi-Largeur
    double half_size;

    Quad(double s) : half_size(s){};

    //À adapter pour le plan
    virtual AABB compute_aabb();
protected:
    //À adapter pour le plan
    virtual bool local_intersect(Ray const ray, double t_min, double t_max, Intersection* hit);
};

// Espace Local: Cylindre tel que l'axe principale est aligné à l'axe Y
//               pour une hauteur (2 * half_height) avec un rayon (radius).
class Cylinder : public Object
{
public:
    //Rayon du cylindre
    double radius;
    //Demi-hauteur du cylindre par rapport à l'origine.
    double half_height;

    Cylinder(double radius, double height) : radius(radius), half_height(height) {};

    //À adapter pour le cylindre
    virtual AABB compute_aabb();
protected:
    //À adapter pour le cylindre
    virtual bool local_intersect(Ray ray, double t_min, double t_max, Intersection* hit);
};

// Une classe pour représenter le sommet d'un polygone. 
// Les entiers stockés sont des indices pour les vecteurs
// positions/tex_coords/normals de l'objet auquel le sommet appartient.
class Vertex {
public:
    // Indices dans les vecteurs positions, tex_coords et normals.
    int pi, ti, ni;

    Vertex() : pi(-1), ti(-1), ni(-1) {}

    Vertex(int pi, int ti, int ni) :
            pi(pi),
            ti(ti),
            ni(ni)
    {}
};

// Un triangle avec 3 sommets contenant les indices associés à l'objet.
class Triangle
{
public:
    Vertex v[3];

    Triangle(Vertex const &v0, Vertex const &v1, Vertex const &v2)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }

    Vertex& operator[](int i) { return v[i]; }
    const Vertex& operator[](int i) const { return v[i]; }
};

// Espace Local: Mesh centrée à l'origine avec les positions spécifiées, normales et les coordonnées de textures
//               associés à chaque triangle.
class Mesh : public Object {
public:
    // Contenant pour les positions, coordonnées de texture, normales et couleurs. Recherche par indice.
    std::vector<double3> positions;
    std::vector<double3> normals;
    std::vector<double2> tex_coords;

    // Les triangles sont des triplets de sommets.
    std::vector<Triangle> triangles;

    // Lis les données OBJ d'un fichier donné.
    Mesh(std::ifstream& file)
    {

        // Continue de récupérer les codes opérationnel et de les analyser. Nous supposons
        // qu'il n'y a qu'une opération par ligne.
        while (file.good()) {

            std::string opString;
            std::getline(file, opString);

            std::stringstream opStream(opString);
            std::string opCode;
            opStream >> opCode;

            // Saute les lignes blanches et les commentaires.
            if (!opCode.size() || opCode[0] == '#') {
                continue;
            }

            // Ignore les groupes.
            if (opCode[0] == 'g' || opCode[0] == 'o' || opCode[0] == 's') {
                std::cerr << "ignored OBJ opCode '" << opCode << "'" << std::endl;
            } // Données de sommet.
            else if (opCode[0] == 'v') {

                // Lis jusqu'à 4 doubles.
                std::vector<double> vec;
                for (int i = 0; opStream.good() && i < 3; i++) {
                    double v;
                    opStream >> v;
                    vec.push_back(v);
                }

                // Stocke cette donnée dans le bon vecteur.
                switch (opCode.size() > 1 ? opCode[1] : 'v') {
                    case 'v':
                        positions.push_back({vec[0],vec[1],vec[2]});
                        break;
                    case 't':
                        tex_coords.push_back({vec[0],vec[1]});
                        break;
                    case 'n':
                        normals.push_back({vec[0],vec[1],vec[2]});
                        break;
                    default:
                        std::cerr << "unknown vertex type '" << opCode << "'" << std::endl;
                        break;
                }
            } // Un polygone (ou face).
            else if (opCode == "f") {
                std::vector<Vertex> polygon;
                // Limite à 4 sommets, puisque nous ne gérons que les triangles ou les quads.
                for (int i = 0; opStream.good() && i < 4; i++) {

                    // Récupère la spécification complète d'un sommet.
                    std::string vertexString;
                    opStream >> vertexString;

                    if (!vertexString.size()) {
                        break;
                    }

                    // Analyse le sommet en un set d'indices pour les positions, coordonnées de tetxure,
                    // normales et couleurs, respectivement.
                    std::stringstream vertexStream(vertexString);
                    std::vector<int> indices;
                    for (int j = 0; vertexStream.good() && j < 3; j++) {
                        // Saute les slashes.
                        if (vertexStream.peek() == '/') {
                            vertexStream.ignore(1);
                        }
                        int index;
                        if (vertexStream >> index)
                            indices.push_back(index);
                    }

                    // Transforme les données récupérées en un véritable sommet, et l'ajoute au polygone.
                    if (indices.size()) {
                        indices.resize(3, 0);
                        polygon.push_back(Vertex(
                                indices[0] - 1,
                                indices[1] - 1,
                                indices[2] - 1));
                    }

                }

                // On n'accepte que les triangles...
                if (polygon.size() == 3) {
                    triangles.push_back(Triangle(polygon[0],
                                                 polygon[1],
                                                 polygon[2]));
                } // ... et les quads...
                else if (polygon.size() == 4) {
                    // ... mais on les décompose en triangle.
                    triangles.push_back(Triangle(polygon[0],
                                                 polygon[1],
                                                 polygon[2]));
                    triangles.push_back(Triangle(polygon[0],
                                                 polygon[2],
                                                 polygon[3]));
                }

                // Tous les autres codes op sont ignorés.
            }
            else {
                std::cerr << "unknown opCode '" << opCode << "'" << std::endl;
            }
        }
    }

    //À adapter pour le mesh
    virtual AABB compute_aabb();
protected:
    //À adapter pour le mesh
    virtual bool local_intersect(Ray const ray, double t_min, double t_max, Intersection* hit);

    // Trouve le point d'intersection entre le rayon donné et le maillage triangulaire.
    // Renvoie true ssi une intersection existe, et remplit les données de
    // la structure hit avec les bonnes informations.
    bool intersect_triangle(Ray const ray,
                            double t_min, double t_max,
                            Triangle const tri,
                            Intersection *hit);
};
