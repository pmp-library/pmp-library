#include "io.h"

using namespace pybind11::literals;

void bind_io(py::module_ &io) {
    io.def("read_to_mesh", &pmp::read, 
        R"pbdoc(
            read(mesh: SurfaceMesh, file: str) -> None

            Read into `mesh` from `file`. SurfaceMesh instance is
            passed as argument.

            Extended Summary
            ----------------
            File extension determines file type. Supported formats and
            vertex attributes (a=ASCII, b=binary):
            
            +--------+--------+---------+------------+---------+-------------+
            | Format | ASCII  | Binary  | Normals    | Colors  | Texcoords   |
            +========+========+=========+============+=========+=============+
            | OBJ    | yes    | no      | a          | no      | no          |
            +--------+--------+---------+------------+---------+-------------+
            | OFF    | yes    | yes     | a / b      | a       | a / b       |
            +--------+--------+---------+------------+---------+-------------+
            | PMP    | no     | yes     | no         | no      | no          |
            +--------+--------+---------+------------+---------+-------------+
            | STL    | yes    | yes     | no         | no      | no          |
            +--------+--------+---------+------------+---------+-------------+

            In addition, the OBJ and PMP formats support reading per-halfedge
            texture coordinates.

            Parameters
            ----------
            mesh : SurfaceMesh
                Surface mesh for writing the data in its structure.
            file : str | bytes | Path
                File name including suffix with file type.
        )pbdoc",
        "mesh"_a, "file"_a
    );
    
    io.def("read", [](const std::filesystem::path& file){
        pmp::SurfaceMesh mesh = pmp::SurfaceMesh();
        pmp::read(mesh, file);
        return mesh;
    }, 
        R"pbdoc(
            read(file: str) -> SurfaceMesh

            Read `file` and return `SurfaceMesh` instance.

            Extended Summary
            ----------------
            File extension determines file type. Supported formats and
            vertex attributes (a=ASCII, b=binary):
            
            +--------+--------+---------+------------+---------+-------------+
            | Format | ASCII  | Binary  | Normals    | Colors  | Texcoords   |
            +========+========+=========+============+=========+=============+
            | OBJ    | yes    | no      | a          | no      | no          |
            +--------+--------+---------+------------+---------+-------------+
            | OFF    | yes    | yes     | a / b      | a       | a / b       |
            +--------+--------+---------+------------+---------+-------------+
            | PMP    | no     | yes     | no         | no      | no          |
            +--------+--------+---------+------------+---------+-------------+
            | STL    | yes    | yes     | no         | no      | no          |
            +--------+--------+---------+------------+---------+-------------+

            In addition, the OBJ and PMP formats support reading per-halfedge
            texture coordinates.

            Parameters
            ----------
            file : str | bytes | Path
                File name including suffix with file type.

            Returns
            -------
            SurfaceMesh
                New SurfaceMesh instance with loaded mesh.
            
        )pbdoc",
        "file"_a
    );

    io.def("write", [](
        const pmp::SurfaceMesh& mesh, 
        const std::filesystem::path& file,
        bool use_binary = false,
        bool use_vertex_normals = false,
        bool use_vertex_colors = false,
        bool use_vertex_texcoords = false,
        bool use_face_normals = false,
        bool use_face_colors = false,
        bool use_halfedge_texcoords = false
    ){
        pmp::IOFlags flags = pmp::IOFlags();
        flags.use_binary = use_binary;
        flags.use_vertex_normals = use_vertex_normals;
        flags.use_vertex_colors = use_vertex_colors;
        flags.use_vertex_texcoords = use_vertex_texcoords;
        flags.use_face_normals = use_face_normals;
        flags.use_face_colors = use_face_colors;
        flags.use_halfedge_texcoords = use_halfedge_texcoords;

        pmp::write(mesh, file, flags);
    },
        R"pbdoc(
            write(mesh: SurfaceMesh, file: str, use_binary: bool = False,
            use_vertex_normals: bool = False, use_vertex_colors: bool = False,
            use_vertex_texcoords: bool = False, use_face_normals: bool = False,
            use_face_colors: bool = False, use_halfedge_texcoords: bool = False
            ) -> None

            Write `mesh` to `file` controlled by flags.

            Extended Summary
            ----------------
            File extension determines file type. Supported formats and
            vertex attributes (a=ASCII, b=binary):
            
            +--------+--------+---------+------------+---------+-------------+
            | Format | ASCII  | Binary  | Normals    | Colors  | Texcoords   |
            +========+========+=========+============+=========+=============+
            | OBJ    | yes    | no      | a          | no      | no          |
            +--------+--------+---------+------------+---------+-------------+
            | OFF    | yes    | yes     | a          | a       | a           |
            +--------+--------+---------+------------+---------+-------------+
            | PMP    | no     | yes     | no         | no      | no          |
            +--------+--------+---------+------------+---------+-------------+
            | STL    | yes    | yes     | no         | no      | no          |
            +--------+--------+---------+------------+---------+-------------+

            In addition, the OBJ and PMP formats support writing per-halfedge
            texture coordinates.

            Parameters
            ----------
            mesh : SurfaceMesh
                Surface mesh for writing the data into the file.
            file : str
                File name including suffix with file type.
            use_binary : bool = False
                Write binary format.
            use_vertex_normals : bool = False
                Write vertex normals.
            use_vertex_colors : bool = False
                Write vertex colors.
            use_vertex_texcoords : bool = False
                Write vertex texcoords.
            use_face_normals : bool = False
                Write face normals.
            use_face_colors : bool = False
                Write face colors.
            use_halfedge_texcoords : bool = False
                Write halfedge texcoords.

        )pbdoc",
        "mesh"_a, "file"_a, "use_binary"_a = false,
        "use_vertex_normals"_a = false, "use_vertex_colors"_a = false,
        "use_vertex_texcoords"_a = false, "use_face_normals"_a = false,
        "use_face_colors"_a = false, "use_halfedge_texcoords"_a = false
    );
}