#pragma once

#include <cfloat>
#include <climits>
#include <cmath>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "basic.h"
#include "bitmap_image/bitmap_image.h"
#include "container.h"
#include "linalg/linalg.h"
#include "resource_manager.h"
#include "scene.h"
using namespace linalg::aliases;

// Different types of tokes that can be lexed
enum TokenType { STRING, NUMBER, NAME, ARRAY_BEGIN, ARRAY_END, END_OF_FILE, ERROR };

// A class to represent a token that has been read
class Token {
   public:
    TokenType type;

    // Variables for different data types
    double number;
    std::string string;

    // Constructors to assign values directly
    Token(TokenType type) : type(type) {}
    Token(double value) : type(NUMBER), number(value) {}
    Token(TokenType type, std::string value) : type(type), string(value) {}

    // Implement the equality operator
    bool operator==(Token const &other) const;
};

// Allow writing a token directly to the output stream
std::ostream &operator<<(std::ostream &out, Token const &token);

// Lexer class
class Lexer {
   public:
    // Constructor. An input stream must be provided
    Lexer(std::istream *input) : _input(input) {}

    // Look at the next token but don't consume it
    Token peek(unsigned int index = 0);

    // Access the next token
    Token next();

    // Skip a certain number of tokens
    void skip(unsigned int count = 1);

    // The following functions will throw an std::string exception if they cannot operate as requested.

    // Get the name of a command
    std::string get_name();

    // Obtain a list of numbers. Min/max refer to the required size of the list
    std::vector<double> get_numbers(unsigned int min = 0, unsigned int max = UINT_MAX);

    // Get a single number
    double get_number();

    // Get a single string
    std::string get_string();

    // Get a list of parameters (i.e. strings mapped to lists of numbers).
    // Min/max apply to each list, just like get_numbers().
    ParamList get_param_list(unsigned int min = 0, unsigned int max = UINT_MAX);
    bitmap_image get_bitmap();

   private:
    // Input stream
    std::istream *_input;

    // Read the input stream and return the next token
    Token _process_stream();

    // Temporary buffer for tokens that have not yet been analyzed
    std::deque<Token> _buffer;
};

class Parser {
   private:
    Lexer lexer;  // The lexer that will be used to analyze the input

    std::vector<double4x4> transform_stack;  // Transformations stack

    std::vector<Object *> objects;

    // The following functions all parse commands that can be found in a .ray file

    // Meta arguments for the scene
    void parse_dimension();
    void parse_samples_per_pixel();
    void parse_jitter_radius();
    void parse_ambient_light();
    void parse_max_ray_depth();

    // Arguments for the camera
    void parse_Perspective();
    void parse_LookAt();
    void parse_DOF();

    // Argument for materials
    void parse_Material();

    // Arguments for object creation with their transformation matrix.
    void parse_PushMatrix();
    void parse_PopMatrix();
    void parse_Translate();
    void parse_Rotate();
    void parse_Scale();

    void parse_Sphere();
    void parse_Quad();
    void parse_Cylinder();
    void parse_Mesh();

    // Arguments for light creation
    void parse_SphericalLight();

    // Analyze the common parts of each object and set up the objects in the scene.
    void finish_object(Object *obj);

   public:
    Scene scene;  // The scene that will be created

    Parser(char const *filename) : lexer(new std::ifstream(filename)) {}
    Parser(std::istream *input) : lexer(input) {}

    ~Parser() {
        if (!scene.container) {
            delete scene.container;
        }

        for (int iobj = 0; iobj < objects.size(); iobj++) {
            delete objects[iobj];
        }
    }

    // Analyze the file or stream passed to the constructor.
    // Save the result in a scene.
    // Returns false on failure; error written to std::cerr.
    bool parse();
};
