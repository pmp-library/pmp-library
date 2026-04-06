"""

        IO - Methods for read/write geometry data in multiple formats
    
"""
from __future__ import annotations
import typing
__all__: list[str] = ['read', 'read_to_mesh', 'write']
def read(file: os.PathLike | str | bytes) -> ...:
    """
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
    """
def read_to_mesh(mesh: ..., file: os.PathLike | str | bytes) -> None:
    """
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
    """
def write(mesh: ..., file: os.PathLike | str | bytes, use_binary: bool = False, use_vertex_normals: bool = False, use_vertex_colors: bool = False, use_vertex_texcoords: bool = False, use_face_normals: bool = False, use_face_colors: bool = False, use_halfedge_texcoords: bool = False) -> None:
    """
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
    """
