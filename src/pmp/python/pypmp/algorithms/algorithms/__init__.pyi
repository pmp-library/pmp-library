"""

        Algorithms - algorithms for PMP mesh manipulation
    
"""
from __future__ import annotations
from . import decimation
from . import differential_geometry
from . import distance_point_triangle
from . import fairing
from . import features
from . import geodesics
from . import hole_filling
from . import normals
from . import parameterization
from . import remeshing
from . import smoothing
from . import subdivision
from . import triangulation
from . import utilities
__all__: list[str] = ['decimation', 'differential_geometry', 'distance_point_triangle', 'fairing', 'features', 'geodesics', 'hole_filling', 'normals', 'parameterization', 'remeshing', 'smoothing', 'subdivision', 'triangulation', 'utilities']
