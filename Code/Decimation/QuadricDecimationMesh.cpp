/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sderstrm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#include "QuadricDecimationMesh.h"

const QuadricDecimationMesh::VisualizationMode QuadricDecimationMesh::QuadricIsoSurfaces = NewVisualizationMode("Quadric Iso Surfaces");

void QuadricDecimationMesh::Initialize()
{
  // Allocate memory for the quadric array
  unsigned int numVerts = mVerts.size();
  mQuadrics.reserve(numVerts);
  std::streamsize width = std::cerr.precision(); // store stream precision
  for (unsigned int i = 0; i < numVerts; i++) {

    // Compute quadric for vertex i here
    mQuadrics.push_back(createQuadricForVert(i));


    // Calculate initial error, should be numerically close to 0

    Vector3<float> v0 = mVerts[i].pos;
    Vector4<float> v(v0[0],v0[1],v0[2],1);
    Matrix4x4<float> m = mQuadrics.back();

    float error = v*(m*v);
    //std::cerr << std::scientific << std::setprecision(2) << error << " ";
  }
  std::cerr << std::setprecision(width) << std::fixed; // reset stream precision

  // Run the initialize for the parent class to initialize the edge collapses
  DecimationMesh::Initialize();
}

/*! \lab2 Implement the computeCollapse here */
/*!
 * \param[in,out] collapse The edge collapse object to (re-)compute, DecimationMesh::EdgeCollapse
 */
void QuadricDecimationMesh::computeCollapse(EdgeCollapse * collapse)
{
	Matrix4x4<float> Q_inv;
  Matrix4x4<float> Q = mQuadrics[ (e(collapse->halfEdge).vert) ];
  Matrix4x4<float> Q_pair = mQuadrics[ (e(e( collapse->halfEdge).pair).vert) ];
	Matrix4x4<float> Q_hat = Q + Q_pair;
  Matrix4x4<float> Q_bar = Q_hat;

  Vector4<float> base_vec = Vector4<float>(0, 0, 0, 1);	
  Q_hat(3,0) = base_vec[0];
	Q_hat(3,1) = base_vec[1];
  Q_hat(3,2) = base_vec[2];
  Q_hat(3,3) = base_vec[3];

	Vector4<float> newPos;

	if(!Q_hat.IsSingular())
	{
	  Q_inv = Q_hat.Inverse();
    newPos = Q_inv * base_vec;
  }
	else{
    Vector3<float> v1 = v(e(collapse->halfEdge).vert).pos;
    Vector3<float> v2 = v(e(e(collapse->halfEdge).pair).vert).pos;

		Vector3<float> v_pos = (v1  + v2) * 0.5f;		
    newPos = Vector4<float>(v_pos[0], v_pos[1], v_pos[2], 1);
  }

  Vector4<float> res = Q_bar * newPos;
  float collapseCost = newPos * res;

	collapse->cost = collapseCost;
	collapse->position = Vector3<float>(newPos[0], newPos[1], newPos[2]);
	
}

/*! After each edge collapse the vertex properties need to be updated */
void QuadricDecimationMesh::updateVertexProperties(unsigned int ind)
{
  DecimationMesh::updateVertexProperties(ind);
  mQuadrics[ind] = createQuadricForVert(ind);
}

/*!
 * \param[in] indx vertex index, points into HalfEdgeMesh::mVerts
*/
Matrix4x4<float> QuadricDecimationMesh::createQuadricForVert(unsigned int indx) const{
  
	float q[4][4] = {{0,0,0,0},
                   {0,0,0,0},
                   {0,0,0,0},
                   {0,0,0,0}};
 
  Matrix4x4<float> Q(q);
  std::vector<unsigned int> faces = FindNeighborFaces(indx);

  for(int i = 0; i < faces.size(); i++){
    Q += createQuadricForFace(faces[i]);
  }
  // The quadric for a vertex is the sum of all the quadrics for the adjacent faces
  // Tip: Matrix4x4 has an operator +=
  return Q;
}

/*!
 * \param[in] indx face index, points into HalfEdgeMesh::mFaces
 */
Matrix4x4<float> QuadricDecimationMesh::createQuadricForFace(unsigned int indx) const{
	Vector3<float> theNormal = f(indx).normal;
  float a = theNormal[0]; 
  float b = theNormal[1];
  float c = theNormal[2];
  float d = -(theNormal * v(e(f(indx).edge).vert).pos);

	float q[4][4] = {{a*a,a*b,a*c,a*d},
                   {b*a,b*b,b*c,b*d},
                   {c*a,c*b,c*c,c*d},
                   {d*a,d*b,d*c,d*d}};

	Matrix4x4<float> Q(q);

  return Q;
}


void QuadricDecimationMesh::Render()
{
  DecimationMesh::Render();

  glEnable(GL_LIGHTING);
  glMatrixMode(GL_MODELVIEW);

  if (mVisualizationMode == QuadricIsoSurfaces)
    {
      // Apply transform
      glPushMatrix(); // Push modelview matrix onto stack

      // Implement the quadric visualization here
      std::cout << "Quadric visualization not implemented" << std::endl;

      // Restore modelview matrix
      glPopMatrix();
    }
}