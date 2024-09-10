#include "error.h"
#include "logger.h"
#include "renderer.h"
#include "mesh.h"
#include "types.h"

using namespace blaz;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        logger.info("Usage: " + str(argv[0]) + " <input_path> <output_path>");
        return 1;
    }

    str input_path = argv[1];
    str output_path = argv[2];

    Mesh obj;
    obj.m_path = input_path;
    Error err = load_mesh_from_obj_file(&obj);
    if (err) {
        logger.error(err);
        return 1;
    }

    err = export_mesh_file(output_path, &obj);
    if (err) {
        logger.error(err);
        return 1;
    }

    return 0;
}