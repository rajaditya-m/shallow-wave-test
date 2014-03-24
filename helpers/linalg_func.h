#ifndef __DYNAMICTEXTURES_LINALGFUNC_H_
#define __DYNAMICTEXTURES_LINALGFUNC_H_

#include <glm/glm.hpp>

//COmputes a.b'
inline glm::mat3 outer_pdk(glm::vec3 a, glm::vec3 b)
{
	glm::mat3 result;
	result[0][0] = a.x * b.x;
	result[0][1] = a.x * b.y;
	result[0][2] = a.x * b.z;
	result[1][0] = a.y * b.x;
	result[1][1] = a.y * b.y;
	result[1][2] = a.y * b.z;
	result[2][0] = a.z * b.x;
	result[2][1] = a.z * b.y;
	result[2][2] = a.z * b.z;
	return result;
}

/*
glm::vec3 solve_lin_sys(glm::mat3 &A,glm::vec3 &b)
{

	glm::vec3 result;

	double arr[9];
	arr[0] = A[0][0]; arr[1] = A[0][1]; arr[2] = A[0][2];
	arr[3] = A[1][0]; arr[4] = A[1][1]; arr[5] = A[1][2];
	arr[6] = A[2][0]; arr[7] = A[2][1]; arr[8] = A[2][2];

	double invA[9];

	double detVal = arr[0]*((arr[4]*arr[8])-(arr[5]*arr[7])) - arr[1]*((arr[3]*arr[8])-(arr[5]*arr[6])) + arr[2]*((arr[3]*arr[7])-(arr[4]*arr[6]));

	if(fabs(detVal-0.0)<1.0e-14)
	{
		std::cout << "[ERROR] System to be solved is singular.\n";
		glm::vec3 placeholder(0.0,0.0,0.0);
		return placeholder;
	}

	double invDetVal = 1.0/detVal;

	//Find out the inverse matrix
	invA[0] = ((arr[4]*arr[8])-(arr[5]*arr[7]))*invDetVal;
	invA[1] = ((arr[2]*arr[7])-(arr[1]*arr[8]))*invDetVal;
	invA[2] = ((arr[1]*arr[5])-(arr[2]*arr[4]))*invDetVal;
	invA[3] = ((arr[5]*arr[6])-(arr[3]*arr[8]))*invDetVal;
	invA[4] = ((arr[0]*arr[8])-(arr[2]*arr[6]))*invDetVal;
	invA[5] = ((arr[2]*arr[3])-(arr[0]*arr[5]))*invDetVal;
	invA[6] = ((arr[3]*arr[7])-(arr[4]*arr[6]))*invDetVal;
	invA[7] = ((arr[1]*arr[6])-(arr[0]*arr[7]))*invDetVal;
	invA[8] = ((arr[0]*arr[4])-(arr[1]*arr[3]))*invDetVal;

	glm::mat3 inverse_mat;
	inverse_mat[0][0] = invA[0];
	inverse_mat[0][1] = invA[1];
	inverse_mat[0][2] = invA[2];
	inverse_mat[1][0] = invA[3];
	inverse_mat[1][1] = invA[4];
	inverse_mat[1][2] = invA[5];
	inverse_mat[2][0] = invA[6];
	inverse_mat[2][1] = invA[7];
	inverse_mat[2][2] = invA[8];

	std::cout << glm::to_string(inverse_mat) << "\n";

	//now multiply the inverse with the values of b to get the solution matrix
	result.x = inverse_mat[0][0]*b.x + inverse_mat[0][1]*b.y + inverse_mat[0][2]*b.z;
	result.y = inverse_mat[1][0]*b.x + inverse_mat[1][1]*b.y + inverse_mat[1][2]*b.z;
	result.z = inverse_mat[2][0]*b.x + inverse_mat[2][1]*b.y + inverse_mat[2][2]*b.z;

	return result;
}
*/

glm::vec3 solve_lin_sys(glm::mat3 &A, glm::vec3 b)
{
	float det_val = glm::determinant(A);

	if(fabs(det_val)<1.0e-14)
	{
		std::cout << "[ERROR] System to be solved is singular.\n";
		glm::vec3 placeholder(0.0,0.0,0.0);
		return placeholder;
	}

	glm::mat3 A_inv = glm::inverse(A);

	// std::cout << glm::to_string(A_inv) << "\n";

	glm::vec3 result;

	result.x = A_inv[0][0]*b.x + A_inv[0][1]*b.y + A_inv[0][2]*b.z;
	result.y = A_inv[1][0]*b.x + A_inv[1][1]*b.y + A_inv[1][2]*b.z;
	result.z = A_inv[2][0]*b.x + A_inv[2][1]*b.y + A_inv[2][2]*b.z;

	return result;
}


#endif
