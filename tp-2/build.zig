const std = @import("std");

// Although this function looks imperative, note that its job is to
// declaratively construct a build graph that will be executed by an external
// runner.
pub fn build(b: *std.Build) void {
    // Standard target options allows the person running `zig build` to choose
    // what target to build for. Here we do not override the defaults, which
    // means any target is allowed, and the default is native. Other options
    // for restricting supported target set are available.
    const target = b.standardTargetOptions(.{});

    // Standard optimization options allow the person running `zig build` to select
    // between Debug, ReleaseSafe, ReleaseFast, and ReleaseSmall. Here we do not
    // set a preferred release mode, allowing the user to decide how to optimize.
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "ray",
        .target = target,
        .optimize = optimize,
    });

    exe.addCSourceFiles(
        .{
            .files = &.{
                "src/main.cpp",
                "src/aabb.cpp",
                "src/container.cpp",
                "src/object.cpp",
                "src/parser.cpp",
                "src/raytracer.cpp",
                "src/resource_manager.cpp",
            },
            .flags = &.{
                // "-Wall",
                // "-std=c++17",
                // "-O1",
                // "-g",
            },
        },
    );

    // Add the library headers to the include path
    exe.linkLibC();
    exe.linkLibCpp();
    exe.addIncludePath(b.path("src/"));
    exe.addIncludePath(b.path("extern/bitmap_image/"));
    exe.addIncludePath(b.path("extern/linalg/"));
    exe.addIncludePath(b.path("extern/pcg32/"));
    b.installArtifact(exe);

    // This *creates* a Run step in the build graph, to be executed when another
    // step is evaluated that depends on it. The next line below will establish
    // such a dependency.
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    // This allows the user to pass arguments to the application in the build
    // command itself, like this: `zig build run -- arg1 arg2 etc`
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    // This creates a build step. It will be visible in the `zig build --help` menu,
    // and can be selected like this: `zig build run`
    // This will evaluate the `run` step rather than the default, which is "install".
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
