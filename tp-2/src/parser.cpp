#include "parser.h"

#define LEX_ERROR(error) { \
    std::stringstream ss; \
    ss << error; \
    throw ss.str(); \
}


bool Token::operator==(Token const& other) const {
    if (type != other.type) {
        return false;
    }
    switch (type) {
    case NUMBER:
        return number == other.number;
    case STRING:
    case NAME:
        return string == other.string;
    }
    return true;
}

std::ostream& operator<<(std::ostream& out, Token const& token) {
    switch (token.type) {
    case STRING:
        out << "STRING:\"" << token.string << "\"";
        break;
    case NUMBER:
        out << "NUMBER:" << token.number;
        break;
    case NAME:
        out << "NAME:" << token.string;
        break;
    case ARRAY_BEGIN:
        out << "ARRAY_BEGIN";
        break;
    case ARRAY_END:
        out << "ARRAY_END";
        break;
    case END_OF_FILE:
        out << "EOF";
        break;
    case ERROR:
        out << "ERROR";
        break;
    default:
        out << "UNKNOWN";
        break;
    }
    return out;
}


Token Lexer::_process_stream(void) {

    // Handle immediate/error conditions.
    if (_input->eof()) {
        return Token(END_OF_FILE);
    }
    if (!_input->good()) {
        return Token(ERROR);
    }

    // The next character in the stream.
    char c = _input->peek();

    // Strip whitespace and comments.
    bool did_strip_something;
    do {
        did_strip_something = false;

        // Strip whitespace.
        while (isspace(c)) {
            _input->ignore(1);
            c = _input->peek();
            did_strip_something = true;
        }

        // Strip comments.
        if (c == '#') {
            do {
                c = _input->get();
            } while (c != '\r' && c != '\n');
            did_strip_something = true;
            c = _input->peek();
        }

    } while (did_strip_something);

    // Arrays.
    switch (c) {
    case '[':
        _input->ignore(1);
        return Token(ARRAY_BEGIN);
    case ']':
        _input->ignore(1);
        return Token(ARRAY_END);
    }

    // Strings.
    if (c == '"') {
        _input->get();
        std::string value;
        bool finished = false;
        while (!finished) {
            c = _input->get();
            switch (c) {
            case '"':
                finished = true;
                break;
                // TODO: handle escapes.
            default:
                value += c;
            }
        }
        return Token(STRING, value);
    }

    // Numbers; try to read one and if it doesn't work reset the error
    // state and carry on.
    double number;
    *_input >> number;
    if (!_input->fail()) {
        return Token(number);
    }
    _input->clear();

    // Names.
    std::string name;
    (*_input) >> name;
    if (name.size()) {
        return Token(NAME, name);
    }

    // There is nothing left to read.
    return Token(END_OF_FILE);
}


Token Lexer::peek(unsigned int index) {
    if (_buffer.size() <= index) {
        _buffer.push_back(_process_stream());
    }
    return _buffer[index];
}


Token Lexer::next() {
    if (!_buffer.size()) {
        return _process_stream();
    }
    Token token = _buffer.front();
    _buffer.pop_front();
    return token;
}


void Lexer::skip(unsigned int count) {
    for (unsigned int i = 0; i < count; i++) {
        next();
    }
}


std::string Lexer::get_name() {
    Token token = peek();
    if (token.type != NAME) {
        LEX_ERROR("expected NAME; got " << token)
    }
    skip(1);
    return token.string;
}


std::vector<double> Lexer::get_numbers(unsigned int min, unsigned int max) {

    std::vector<double> values;

    bool is_array = peek().type == ARRAY_BEGIN;
    if (is_array) {
        skip(1);
    }

    // UGLY HACK: Only here so we have something to `continue` to.
    do {

        Token token = next();

        switch (token.type) {
        case NUMBER:
            values.push_back(token.number);
            if (!is_array) {
                break;
            }
            continue;

        case ARRAY_END:

            if (is_array) {
                break;
            }
            // falling through here
        default:
            LEX_ERROR("expected NUMBER; got " << token);
        }

        if (values.size() >= min && values.size() <= max) {
            return values;
        }
        LEX_ERROR("expected " << min << " to " << max << " NUMBERs; got " << values.size())

    } while (true);

}


double Lexer::get_number() {
    return get_numbers(1, 1)[0];
}


std::string Lexer::get_string() {
    Token token = peek();
    if (token.type != STRING) {
        LEX_ERROR("expected STRING; got " << token)
    }
    skip(1);
    return token.string;
}

bitmap_image Lexer::get_bitmap() {
    std::string t;
    bitmap_image m;
    t = get_string();
    if (t.length() == 0) {
        return bitmap_image();
    }
    m = bitmap_image(t);
    return m;
}

ParamList Lexer::get_param_list(unsigned int min, unsigned int max) {
    ParamList map;
    while (true) {
        std::string key;
        try {
            key = get_string();
        }
        catch (std::string) {
            return map;
        }
        map[key] = get_numbers(min, max);
    }
}


bool Parser::parse() {

    transform_stack.push_back(linalg::identity);

    std::string container;

    while (true) {

        Token token = lexer.peek();
        switch (token.type) {
        case END_OF_FILE:
            if (container == "BVH") {
                scene.container = new BVH(objects);
            }
            else if (container == "Naive") {
                scene.container = new Naive(objects);
            }

            return true;
        case ERROR:
            std::cerr << "parsing failed due to lexing error" << std::endl;
            return false;
        }


        std::string name;
        try {
            name = lexer.get_name();
        }
        catch (std::string e) {
            std::cerr << "parsing failed to get next command due to: " << e << std::endl;
            return false;
        }

#define HANDLE_NAME(_name) if (name == #_name) { parse_##_name(); continue; }

        try {
            HANDLE_NAME(dimension)
                HANDLE_NAME(jitter_radius)
                HANDLE_NAME(samples_per_pixel)
                HANDLE_NAME(ambient_light)
                HANDLE_NAME(max_ray_depth)
                HANDLE_NAME(jitter_radius)


                HANDLE_NAME(Perspective)
                HANDLE_NAME(LookAt)
                HANDLE_NAME(DOF)

                HANDLE_NAME(Material)

                HANDLE_NAME(PushMatrix)
                HANDLE_NAME(PopMatrix)
                HANDLE_NAME(Translate)
                HANDLE_NAME(Scale)
                HANDLE_NAME(Rotate)

                HANDLE_NAME(Sphere)
                HANDLE_NAME(Quad)
                HANDLE_NAME(Mesh)
                HANDLE_NAME(Cylinder)

                HANDLE_NAME(SphericalLight)

                if (name == "container") {
                    container = lexer.get_string();

                    if (!(container == "BVH" || container == "Naive")) {
                        std::cerr << "parsing failed due to unknown container \"" << container << "\"" << std::endl;
                        return false;
                    }

                    continue;
                }

        }
        catch (std::string e) {
            std::cerr << "parsing failed on command \"" << name << "\" due to: " << e << std::endl;
            return false;
        }

        // Should have `continue`d by this point.
        std::cerr << "parsing failed due to unknown command \"" << name << "\"" << std::endl;
        return false;
    }
}

void Parser::parse_dimension() {
    // dimensions
    scene.resolution[0] = static_cast<int>(lexer.get_number());
    scene.resolution[1] = static_cast<int>(lexer.get_number());
}

void Parser::parse_samples_per_pixel() {
    scene.samples_per_pixel = lexer.get_number();
}

void Parser::parse_jitter_radius() {
    scene.jitter_radius = lexer.get_number();
}

void Parser::parse_ambient_light() {
    std::vector<double> values = lexer.get_numbers(3, 3);
    scene.ambient_light = { values[0],values[1],values[2] };
}

void Parser::parse_max_ray_depth() {
    scene.max_ray_depth = static_cast<int>(lexer.get_number());
}

void Parser::parse_Perspective() {
    scene.camera.fovy = lexer.get_number();
    scene.camera.aspect = lexer.get_number();
    scene.camera.z_near = lexer.get_number();
    scene.camera.z_far = lexer.get_number();
}


void Parser::parse_LookAt() {
    scene.camera.position[0] = lexer.get_number();
    scene.camera.position[1] = lexer.get_number();
    scene.camera.position[2] = lexer.get_number();
    scene.camera.center[0] = lexer.get_number();
    scene.camera.center[1] = lexer.get_number();
    scene.camera.center[2] = lexer.get_number();
    scene.camera.up[0] = lexer.get_number();
    scene.camera.up[1] = lexer.get_number();
    scene.camera.up[2] = lexer.get_number();
}

void Parser::parse_DOF() {
    scene.camera.defocus_angle = lexer.get_number();
    scene.camera.focus_distance = lexer.get_number();
}

void Parser::parse_Material() {
    std::string name = lexer.get_string();
    lexer.get_string();
    bitmap_image m = lexer.get_bitmap();
    ParamList params = lexer.get_param_list(1, 4);
    ResourceManager::Instance()->materials[name] = Material(m, params);

}

void Parser::parse_PopMatrix() {
    transform_stack.pop_back();
    if (!transform_stack.size()) {
        throw std::string("popped off bottom of stack");
    }
}


void Parser::parse_PushMatrix() {
    transform_stack.push_back(transform_stack.back());
}


void Parser::parse_Translate() {
    // Need to store these in variables because if we pass them directly to
    // Matrix::whatever(...) we are no guarunteed they are called in order.
    double x = lexer.get_number();
    double y = lexer.get_number();
    double z = lexer.get_number();
    transform_stack.back() = mul(transform_stack.back(), linalg::translation_matrix(double3{ x, y, z }));
}


void Parser::parse_Scale() {
    // Need to store these in variables because if we pass them directly to
    // Matrix::whatever(...) we are no guarunteed they are called in order.
    double x = lexer.get_number();
    double y = lexer.get_number();
    double z = lexer.get_number();
    transform_stack.back() = mul(transform_stack.back(), linalg::scaling_matrix(double3{ x, y, z }));
}


void Parser::parse_Rotate() {
    // Need to store these in variables because if we pass them directly to
    // Matrix::whatever(...) we are no guarunteed they are called in order.
    double a = lexer.get_number();
    double x = lexer.get_number();
    double y = lexer.get_number();
    double z = lexer.get_number();

    transform_stack.back() = mul(transform_stack.back(), linalg::rotation_matrix(linalg::rotation_quat(double3{ x, y, z }, deg2rad(a))));
}


void Parser::parse_Sphere() {
    double radius = lexer.get_number();

    Sphere* obj = new Sphere(radius);
    finish_object(obj);
}


void Parser::parse_Quad() {
    double size = lexer.get_number();

    Quad* obj = new Quad(size);
    finish_object(obj);
}

void Parser::parse_Cylinder() {
    double radius = lexer.get_number();
    double height = lexer.get_number();


    Cylinder* obj = new Cylinder(radius, height);
    finish_object(obj);
}

void Parser::parse_Mesh() {

    std::cout << "Mesh: ";

    // First try to get a filename, and read an OBJ from it.
    std::string filename;
    try {
        filename = lexer.get_string();
        std::cout << "got OBJ filename \"" << filename << "\"" << std::endl;

        std::ifstream file(filename.c_str());
        if (!file.good()) {
            throw std::string("Unable to open OBJ file: ") + filename;
        }

        Mesh* obj = new Mesh(file);
        std::cout << obj->triangles.size() << " triangles" << std::endl;

        finish_object(obj);
    }
    catch (std::string e) {
        // OK.
        std::cout << "Could not be parse :: " << e << std::endl;
    }

    // Normally this comes last, but we want the color on the front.

}

void Parser::finish_object(Object* obj) {

    // Get the material name, and make sure that material exists.
    std::string material_name = lexer.get_string();
    if (!ResourceManager::Instance()->materials.count(material_name)) {
        std::stringstream ss;
        ss << "no material \"" << material_name << "\"" << std::endl;
        throw(ss.str());
    }
    obj->key_material = material_name;

    // Set transform, inv transform, and normal transform.
    obj->setup_transform(transform_stack.back());

    // Add to the list of objects.
    objects.push_back(obj);
}

void Parser::parse_SphericalLight() {
    double3 position;
    position[0] = lexer.get_number();
    position[1] = lexer.get_number();
    position[2] = lexer.get_number();
    ParamList params = lexer.get_param_list(1, 4);

    scene.lights.push_back(SphericalLight(position, params));
}