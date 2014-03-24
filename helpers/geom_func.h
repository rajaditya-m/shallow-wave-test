#ifndef __DYNAMICTEXTURES_GEOMFUNC_H
#define __DYNAMICTEXTURES_GEOMFUNC_H

#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp" //Delete later

glm::vec4 get_center_mass(glm::vec4 a, glm::vec4 b, glm::vec4 c)
{
	glm::vec4 com = (a+b+c)/3;
	return com;
}

bool same_side_vector(glm::vec3 p1, glm::vec3 p2, glm::vec3 vs,glm::vec3 ve,float *ret_val)
{
	glm::vec3 v = ve-vs;
	glm::vec3 q1 = p1-vs;
	glm::vec3 q2 = p2-vs;
	glm::vec3 cp1 = glm::cross(v,q1);
	glm::vec3 cp2 = glm::cross(v,q2);
	float dot_pdk_val = glm::dot(cp1,cp2);
	if(dot_pdk_val>0 || fabs(dot_pdk_val)<0.0000001)
	{
		*ret_val = dot_pdk_val;
		return true;
	}
	else 
		return false;
}

bool inside_triangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	//First calculate the cross product 
	glm::vec3 ABxAC = glm::cross(b-a,c-a);

	//Next do inside outside test with edgeAB 
	glm::vec3 AB = b-a;
	glm::vec3 AP = p-a;
	glm::vec3 ABxAP = glm::cross(AB,AP);
	float ut = glm::dot(ABxAP,ABxAC);
	if(ut<0) 
		return false;

	//Next do inside outside test with edgeBC
	glm::vec3 BC = c-b;
	glm::vec3 BP = p-b;
	glm::vec3 BCxBP = glm::cross(BC,BP);
	float wt = glm::dot(BCxBP,ABxAC);
	if(wt<0)
		return false;

	//Next do inside outside test with edgeCA
	glm::vec3 CA = a-c;
	glm::vec3 CP = p-c;
	glm::vec3 CAxCP = glm::cross(CA,CP);
	float vt = glm::dot(CAxCP,ABxAC);
	if(vt<0)
		return false;

	return true;
}

void get_shortest_distance_to_triangle(glm::vec3 t1,glm::vec3 t2,glm::vec3 t3,glm::vec3 P,glm::vec3 *Q, float *dist,float *s_f,float *t_f)
{
	//Parametric Reprensetation of triangle
	glm::vec3 B = t1;
	glm::vec3 E0 = t2-t1;
	glm::vec3 E1 = t3-t1;

	//compute quantities used in the code
	glm::vec3 D = B-P;

	float a = glm::dot(E0,E0);
	float b = glm::dot(E0,E1);
	float c = glm::dot(E1,E1);
	float d = glm::dot(E0,D);
	float e = glm::dot(E1,D);
	float f = glm::dot(D,D);

	//Now the actual computation 
	float det = a*c - b*b;
	float s = b*e - c*d;
	float t = b*d - a*e;

	if(s+t<=det)
	{
		if(s<0)
		{
			if(t<0)
			{
				float tmp0 = b+d;
				float tmp1 = c+e;
				if(tmp1>tmp0)
				{
					float numer = tmp1-tmp0;
					float denom = a-2*b+c;
					s = (numer >= denom ? 1 : numer/denom);
					t = 1-s;
				}
				else
				{
					s=0;
					t = (tmp1<=0 ? 1: (e >= 0 ? 0 : -e/c));
				}
			}
			else
			{
				s = 0;
				t = (e >= 0 ? 0 :(-e >=c ? 1 : -e/c));
			}
		}
		else if(t<0)
		{
			s = 0;
			t = (e >= 0 ? 0 :(-e >=c ? 1 : -e/c));
		}
		else
		{
			float inv_det = 1/det;
			s *= inv_det;
			t *= inv_det; 
		}
	}
	else
	{
		if(s<0)
		{
			float tmp0 = b+d;
			float tmp1 = c+e;
			if(tmp1>tmp0)
			{
				float numer = tmp1-tmp0;
				float denom = a-2*b+c;
				s = (numer >= denom ? 1 : numer/denom);
				t = 1-s;
			}
			else
			{
				s=0;
				t = (tmp1<=0 ? 1: (e >= 0 ? 0 : -e/c));
			}
		}
		else if(t<0)
		{
			float tmp0 = b+d;
			float tmp1 = c+e;
			if(tmp1>tmp0)
			{
				float numer = tmp1-tmp0;
				float denom = a-2*b+c;
				s = (numer >= denom ? 1 : numer/denom);
				t = 1-s;
			}
			else
			{
				s=0;
				t = (tmp1<=0 ? 1: (e >= 0 ? 0 : -e/c));
			}
		}
		else
		{
			float numer = (c+e)-(b+d);
			if(numer <=0)
			{
				s=0;
			}
			else
			{
				float denom = a-2*b+c;
				s = (numer>=denom ? 1 : numer/denom);
			}
			t = 1-s;
		}
	}

	*Q = B + s*E0 + t*E1;
	*dist = glm::length(*Q-P);
	*s_f = s;
	*t_f = t;
}

#endif
