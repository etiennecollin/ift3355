#include "raytracer.h"

#include "basic.h"

void Raytracer::render(const Scene& scene, Frame* output) {
    // Create the z_buffer
    double* z_buffer = new double[scene.resolution[0] * scene.resolution[1]];
    for (int i = 0; i < scene.resolution[0] * scene.resolution[1]; i++) {
        z_buffer[i] = scene.camera.z_far;  // Anciennement DBL_MAX. À remplacer avec la valeur de scene.camera.z_far
    }

    // CameraOrthographic camOrth;
    // double3 uVec{0, 1, 0};
    // double3 vVec{0, 0, 1};
    // double y_shift = 2.0 / scene.resolution[1];
    // double x_shift = 2.0 / scene.resolution[0];
    //
    // for (int y = 0; y < scene.resolution[1]; y++) {
    //     if (y % 40) {
    //         std::cout << "\rScanlines completed: " << y << "/" << scene.resolution[1] << '\r';
    //     }
    //
    //     for (int x = 0; x < scene.resolution[0]; x++) {
    //         double3 color{0, 0, 0};
    //
    //         Intersection hit;
    //         double3 rayOrigin = camOrth.minPosition + uVec * x_shift * x + vVec * y_shift * y;
    //         double3 rayDirection{1, 0, 0};
    //         Ray ray = Ray(rayOrigin, rayDirection);
    //         double itHits = 0;
    //
    //         double z_depth = scene.camera.z_far;
    //         if (scene.container->intersect(ray, EPSILON, z_depth, &hit)) {
    //             Material& material = ResourceManager::Instance()->materials[hit.key_material];
    //             color = material.color_albedo;
    //             itHits = 1.0f;
    //         }
    //
    //         output->set_color_pixel(x, y, color);
    //         output->set_depth_pixel(x, y, itHits);
    //     }
    // }

    //---------------------------------------------------------------------------------------------------------------

    // Get the center and right vectors which point in the direction of the camera view and the right of the camera
    // respectively.
    double3 CENTER = normalize(scene.camera.center - scene.camera.position);
    double3 RIGHT = normalize(cross(CENTER, scene.camera.up));
    double3 UP = normalize(cross(RIGHT, CENTER));

    // Screen is placed at z-near distance from the camera or at the focus distance if it is set
    double screen_depth = scene.camera.focus_distance > 0 ? scene.camera.focus_distance : scene.camera.z_near;

    // Get the offset of the top left corner of the screen
    double screen_top_offset = tan(deg2rad(scene.camera.fovy / 2)) * screen_depth;
    double screen_left_offset = screen_top_offset * scene.camera.aspect;

    // Get the size of a pixel in the screen space
    double pixel_size_x = 2 * screen_left_offset / scene.resolution[0];
    double pixel_size_y = 2 * screen_top_offset / scene.resolution[1];

    // Get the world coordinates of the top left corner of the screen
    double3 screen_tl_position = scene.camera.position - (screen_left_offset - pixel_size_x / 2) * RIGHT -
                                 (screen_top_offset - pixel_size_y / 2) * UP + screen_depth * CENTER;

    // Compute the angle between the defocus cone axis and side
    double defocus_cone_angle = deg2rad(scene.camera.defocus_angle / 2);
    // Compute the radius at the base of the cone
    double defocus_radius = scene.camera.focus_distance * tan(defocus_cone_angle);

    // Iterate on all pixels
    auto render_row = [&](int y) {
        for (int x = 0; x < scene.resolution[0]; x++) {
            int avg_z_depth = 0;
            double3 avg_ray_color{0, 0, 0};

            // Generate multiple samples per pixel
            for (int iray = 0; iray < scene.samples_per_pixel; iray++) {
                // Initialize ray
                Ray ray;

                // Set the origin of the ray to the camera position with the focus offset
                double3 focus_offset = {0, 0, 0};
                if (scene.camera.defocus_angle > 0) {
                    // Generate the random disk point and compute offset
                    double2 random_disk_point = random_in_unit_disk() * defocus_radius;
                    focus_offset = random_disk_point.x * RIGHT + random_disk_point.y * UP;
                }
                ray.origin = scene.camera.position + focus_offset;

                // Compute the pixel position with a random jitter
                double2 pixel_jitter = (rand_double2() - 0.5) * 2 * scene.jitter_radius;
                double3 pixel_x_offset = (x + pixel_jitter.x) * pixel_size_x * RIGHT;
                double3 pixel_y_offset = (y + pixel_jitter.y) * pixel_size_y * UP;
                double3 pixel_position = screen_tl_position + pixel_x_offset + pixel_y_offset;

                // Compute the direction of the ray
                ray.direction = normalize(pixel_position - ray.origin);

                // Initialize the tracing
                Intersection hit;
                double ray_hit_depth = scene.camera.z_far;
                int ray_depth = 0;
                double3 ray_color{0, 0, 0};

                // Trace the ray
                trace(scene, ray, ray_depth, &ray_color, &ray_hit_depth);

                // Add the color and depth to the averages
                avg_ray_color += ray_color;
                avg_z_depth += ray_hit_depth;
            }

            // Compute the average color and depth
            avg_z_depth = avg_z_depth / scene.samples_per_pixel;
            avg_ray_color = avg_ray_color / scene.samples_per_pixel;

            // Test if depth is valid
            if (avg_z_depth >= scene.camera.z_near && avg_z_depth <= scene.camera.z_far &&
                avg_z_depth < z_buffer[x + y * scene.resolution[0]]) {
                z_buffer[x + y * scene.resolution[0]] = avg_z_depth;

                // Update the pixel color and depth
                output->set_color_pixel(x, y, avg_ray_color);
                output->set_depth_pixel(
                    x, y, (avg_z_depth - scene.camera.z_near) / (scene.camera.z_far - scene.camera.z_near));
            }
        }
    };

    std::vector<std::future<void>> futures;

    // Launch threads for each row
    for (int y = 0; y < scene.resolution[1]; y++) {
        futures.push_back(std::async(std::launch::async, render_row, y));
    }

    // Wait for all threads to finish
    for (auto& future : futures) {
        future.get();
    }

    delete[] z_buffer;
}

// Determine if the ray intersects an object
//   - Compute the contribution associated with reflection
//   - Compute the contribution associated with refraction
//   - Update the color with the shading + reflection according to material.reflection + refraction according to
//      material.refraction
//   - Update the new depth
void Raytracer::trace(const Scene& scene, Ray ray, int ray_depth, double3* out_color, double* out_z_depth) {
    Intersection hit;
    // Fait appel à l'un des containers spécifiées.
    if (scene.container->intersect(ray, EPSILON, *out_z_depth, &hit)) {
        Material mat = ResourceManager::Instance()->materials[hit.key_material];
        double3 reflected_color = {0, 0, 0};
        double3 refracted_color = {0, 0, 0};

        // Limit the number of rays
        if (ray_depth < scene.max_ray_depth) {
            double dot_product;
            double3 incident_direction = -ray.direction;
            if (mat.k_reflection != 0 || mat.k_refraction != 0) {
                // Compute the dot product once as it is used multiple times
                dot_product = dot(hit.normal, incident_direction);
            }
            if (mat.k_reflection != 0) {
                // Compute direction of reflected ray
                double3 reflected_direction = normalize(2 * dot_product * hit.normal - incident_direction);

                // Reflected ray starts at hit and goes in the reflected direction
                Ray reflected_ray = Ray(hit.position, reflected_direction);
                reflected_ray.ior_stack = ray.ior_stack;

                // Trace the reflected ray
                trace(scene, reflected_ray, ray_depth + 1, &reflected_color, out_z_depth);
            }
            if (mat.k_refraction != 0) {
                std::vector<double2> new_ior_stack = ray.ior_stack;

                // Check if we are exiting the object and set proper eta
                double eta;
                if (new_ior_stack.back().x == hit.obj_id) {
                    // We are exiting the object
                    new_ior_stack.pop_back();
                    eta = mat.refractive_index / new_ior_stack.back().y;
                } else {
                    // We are entering the object
                    eta = new_ior_stack.back().y / mat.refractive_index;
                    new_ior_stack.push_back({hit.obj_id, mat.refractive_index});
                }

                // Check for total internal reflection
                double3 direction;
                double root = sqrt(1 - pow(dot_product, 2));
                if (eta * root > 1) {
                    // Reflect the ray
                    direction = normalize(2 * dot_product * hit.normal - incident_direction);
                    // We are still inside the object
                    new_ior_stack.push_back({hit.obj_id, mat.refractive_index});
                } else {
                    // Refract the ray
                    direction =
                        normalize(hit.normal * (eta * dot_product - sqrt(1 - pow(eta, 2) * (1 - pow(dot_product, 2)))) -
                                  eta * incident_direction);
                }

                // Refracted ray starts at hit and goes in the refracted direction
                Ray refracted_ray = Ray(hit.position, direction);
                refracted_ray.ior_stack = new_ior_stack;

                // Trace the refracted ray
                trace(scene, refracted_ray, ray_depth + 1, &refracted_color, out_z_depth);
            }
        }

        // Get the final color and depth
        *out_color = shade(scene, hit) + mat.k_reflection * reflected_color + mat.k_refraction * refracted_color;
        *out_z_depth = hit.depth;
    }
}

// Compute the contribution of the lights in the scene.
//    - Iterate over all lights
//      - Include the specular contribution using the Blinn model including the metallic component
//      - Include the diffuse contribution
//    - Include the ambient contribution
// Compute if the point is in shadow
//   - Iterate over all objects and detect if the ray between the intersection and the light is occluded
//      - Do not consider points further than the light
//   - Integrate the penumbra in your calculation
// Determine the color of the intersection point
//  - If texture is present, take the color at the uv coordinates
//  - If no texture, take the color associated with the material
double3 Raytracer::shade(const Scene& scene, Intersection hit) {
    Material& mat = ResourceManager::Instance()->materials[hit.key_material];
    double3 albedo;

    // Iterate on all lights
    double3 light_sum = {0, 0, 0};
    for (int i = 0; i < scene.lights.size(); i++) {
        SphericalLight light = scene.lights[i];

        // Compute some important distances and directions
        double hit_light_distance = length(light.position - hit.position);
        double3 hit_light_direction = normalize(light.position - hit.position);
        double3 hit_camera_direction = normalize(scene.camera.position - hit.position);

        // Check if the point is in shadow
        // If the light is directional
        double occlusion_factor = 1;
        if (light.radius == 0) {
            // Throw a ray from the hit point to the light
            // If it intersects an object, the point is in shadow
            Intersection shadow_hit;
            Ray shadow_ray = Ray{hit.position, hit_light_direction};
            if (scene.container->intersect(shadow_ray, EPSILON, hit_light_distance, &shadow_hit)) {
                occlusion_factor = 0;
            }
        } else {
            // Set the number of rays to cast within the cone
            int num_shadow_rays = scene.samples_per_pixel;
            int rays_reaching_light = 0;

            // Calculate the cone angle using the light radius and distance to the intersection point
            double cone_angle = atan2(light.radius, hit_light_distance);

            // Define the coordinate system for the cone
            // u and v are perpendicular to the cone axis and each other
            double3 u, v;
            // Make sure the cone axis is not parallel to the up
            if (fabs(hit_light_direction.x) > 0.1) {
                u = normalize(cross({0, 1, 0}, hit_light_direction));
            } else {
                u = normalize(cross({1, 0, 0}, hit_light_direction));
            }
            v = cross(hit_light_direction, u);

            // Cast multiple rays within the cone
            for (int j = 0; j < num_shadow_rays; j++) {
                // Sample a random point in the unit disk for cone sampling
                double2 disk_sample = random_in_unit_disk();

                // Map the disk sample to the cone angle
                double sample_x = disk_sample.x * sin(cone_angle);
                double sample_y = disk_sample.y * sin(cone_angle);
                double sample_z = cos(cone_angle);

                // Convert the sample to world space using the cone basis
                double3 sample_direction = normalize(sample_x * u + sample_y * v + sample_z * hit_light_direction);

                // Cast a shadow ray from hit point in the sampled direction
                Ray shadow_ray = Ray{hit.position, sample_direction};
                Intersection shadow_hit;
                if (scene.container->intersect(shadow_ray, EPSILON, hit_light_distance, &shadow_hit)) {
                    continue;
                }
                rays_reaching_light++;
            }

            // Compute the occlusion factor as the ratio of rays reaching the light
            occlusion_factor = static_cast<double>(rays_reaching_light) / num_shadow_rays;
        }

        // Pre-compute the N dot L product
        double n_dot_l = fmax(0, dot(hit.normal, hit_light_direction));

        // Pre-compute the N dot H product for Blinn
        double3 h = normalize(hit_light_direction + hit_camera_direction);
        double n_dot_h = fmax(0, dot(hit.normal, h));

        // Select albedo
        bitmap_image texture = mat.texture_albedo;
        if (texture.width() != 0 && texture.height() != 0) {
            rgb_t pixel =
                mat.texture_albedo.get_pixel(floor(hit.uv.x * texture.width()), floor(hit.uv.y * texture.height()));

            albedo = {static_cast<double>(pixel.red), static_cast<double>(pixel.green),
                      static_cast<double>(pixel.blue)};
            // Normalize the color
            for (int i = 0; i < 3; i++) {
                albedo[i] /= 255;
            }
        } else {
            albedo = mat.color_albedo;
        }

        // Get the diffuse contribution
        double3 diffuse = mat.k_diffuse * albedo * n_dot_l;

        // Get the specular contribution
        double3 specular = mat.k_specular * (mat.metallic * albedo + (1 - mat.metallic)) * pow(n_dot_h, mat.shininess);

        // Add the contribution to the total
        light_sum += occlusion_factor * (light.emission / pow(hit_light_distance, 2)) * (diffuse + specular);
    }

    // Get the ambient contribution
    double3 ambient = scene.ambient_light * mat.k_ambient * albedo;

    // Return the final color
    return ambient + light_sum;
    // return hit.normal;
}
