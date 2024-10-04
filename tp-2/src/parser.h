#pragma once


#include <climits>
#include <deque>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <cfloat>

#include "bitmap_image/bitmap_image.h"
#include "scene.h"
#include "basic.h"
#include "container.h"

#include "resource_manager.h"

#include "linalg/linalg.h"
using namespace linalg::aliases;

// Les différents types de token pouvant être lexés
enum TokenType {
    STRING,
    NUMBER,
    NAME,
    ARRAY_BEGIN,
    ARRAY_END,
    END_OF_FILE,
    ERROR
};


// Une classe pour représenter un token qui a été lu
class Token
{
public:
    TokenType type;

    // Variables pour les différents types de données.
    double number;
    std::string string;

    // Quelques constructeurs pour assigner directement des valeurs.
    Token(TokenType type) : type(type) {}
    Token(double value) : type(NUMBER), number(value) {}
    Token(TokenType type, std::string value) : type(type), string(value) {}

    // Opérateur d'égalité.
    bool operator==(Token const &other) const;
};


// Permet l'écriture d'un token directement dans le flux de sortie.
std::ostream& operator<<(std::ostream &out, Token const &token);


// La classe du lexer.
class Lexer
{
public:
    // Constructeur. Un flux d'entrée doit être fourni.
    Lexer(std::istream *input) : _input(input) {}

    // Regarde le prochain token mais ne le consomme pas.
    Token peek(unsigned int index = 0);

    // Accède au token suivant.
    Token next();

    // Passe un certain nombre de tokens.
    void skip(unsigned int count = 1);

    // Les fonctions suivantes produiront une exception std::string si
    // elles ne peuvent pas fonctionner comme demandé.

    // Récupère un nom de commande.
    std::string get_name();

    // Récupère une liste de nombres. Min/max font référence
    // à la taille requise de la liste.
    std::vector<double> get_numbers(unsigned int min = 0,
                                   unsigned int max = UINT_MAX);

    // Récupère un unique nombre.
    double get_number();

    // Récupère une unique string.
    std::string get_string();

    // Récupère une liste de paramètres (i.e. strings mappés à des listes de nombres).
    // Min/max s'appliquent à chaque liste, tout comme get_numbers().
    ParamList get_param_list(unsigned int min = 0,
                           unsigned int max = UINT_MAX);
    bitmap_image get_bitmap();
private:
    // Le flux d'entrée.
    std::istream *_input;

    // Fonction pour lire le flux d'entrée et retourner le token suivant.
    Token _process_stream();

    // Un buffer temporaire pour les tokens qui n'ont pas encore été analysés.
    std::deque<Token> _buffer;
};


class Parser
{
private:
    Lexer lexer; // Le lexer utilisé pour séparer le fichier en tokens.

    std::vector<double4x4> transform_stack;  // Pile de transformations.

    std::vector<Object*> objects;

    // Les fonctions suivantes analysent toutes les commandes qui peuvent être
    // trouvées dans un fichier .ray.

    //Meta argument pour la scene
    void parse_dimension();
    void parse_samples_per_pixel();
    void parse_jitter_radius();
    void parse_ambient_light();
    void parse_max_ray_depth();

    //Argument pour la caméra
    void parse_Perspective();
    void parse_LookAt();
    void parse_DOF();

    //Argument pour les matériaux
    void parse_Material();

    //Argument pour la création d'objet avec leur matrice de transformation.
    void parse_PushMatrix();
    void parse_PopMatrix();
    void parse_Translate();
    void parse_Rotate();
    void parse_Scale();

    void parse_Sphere();
    void parse_Quad();
    void parse_Cylinder();
    void parse_Mesh();

    //Argument pour la création de lumière
    void parse_SphericalLight();

    // Analyse les parties communes de chaque objet et met en place les objets dans la scène.
    void finish_object(Object *obj);

public:
    Scene scene; // La scène qui sera créée pendant l'analyse.

    Parser(char const * filename) : lexer(new std::ifstream(filename)) {}
    Parser(std::istream *input) : lexer(input) {}

    ~Parser() {
        if(!scene.container){
            delete scene.container;
        }

        for(int iobj = 0; iobj < objects.size(); iobj++) {
            delete objects[iobj];
        }
    }

    // Analyse le fichier ou le flux passé au constructeur.
    // Sauvegarde le résultat dans une scène.
    // Retourne false sur un échec ; erreur écrite dans std::cerr.
    bool parse();
};