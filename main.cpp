#include "Oracle.h"

#include "SOIL.h"

#include <glm/gtc/swizzle.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include "glm/gtx/string_cast.hpp" //Delete later

//Eigen classes
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/OrderingMethods>
#include <unsupported/Eigen/SparseExtra> //Experimental unsupp=orted modules !!!!

#include <vector>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <numeric>

#define RAD2DEG 57.29

#define R 8.314

#define GRID_SIZE 10

#define DAMPING 0.9999
#define RATE 0.0005

typedef enum {XFORM_NONE,XFORM_ROTATE,XFORM_SCALE} TransformType;
TransformType xform_mode = XFORM_NONE; 
GLfloat x_angle=0.0,y_angle=0.0,z_angle=0.0;
int press_x, press_y; 
int release_x, release_y; 
float scale_size = 1.0; 

float height_prev[GRID_SIZE][GRID_SIZE];
float height_current[GRID_SIZE][GRID_SIZE];
float height_new[GRID_SIZE][GRID_SIZE];

glm::vec3 point_set[GRID_SIZE][GRID_SIZE];
glm::vec3 normal_set[GRID_SIZE][GRID_SIZE];

float areas[GRID_SIZE][GRID_SIZE];


void init_fields()
{
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			height_current[i][j] = 1.0;
			height_prev[i][j] = 1.0;
			areas[i][j] = 0.0;
		}
	}
	//Calculate areas
	for(int i=0;i<GRID_SIZE-1;i++)
	{
		for(int j=0;j<GRID_SIZE-1;j++)
		{
			//Triangle # i,j
			float area_unit = 1.0;
			areas[i][j] += 0.25;
			areas[i+1][j] += 0.25;
			areas[i][j+1] += 0.25;
			areas[i+1][j+1] += 0.25;
		}
	}
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			std::cout << areas[i][j] << " ";
		}
		std::cout << "\n";
	}
}

void material_setup()
{
	// Initialise and set material parameters
	GLfloat material_ka[] = {0.1, 0.18725, 0.1745, 1.0};
	GLfloat material_kd[] = {0.396,0.74151,0.69102,1.0};
	GLfloat material_ks[] = {0.297254,0.30829,0.306678,1.0};
	GLfloat material_ke[] = {0.0, 0.0, 0.0, 0.0};
	GLfloat material_se[] = {12.8};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  material_ka);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  material_kd);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  material_ks);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  material_ke);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_se);
}


void draw_single_bar(float x, float y,float h)
{
	glPushMatrix();

	glEnable (GL_LIGHTING);

	glEnable(GL_NORMALIZE);

	glRotatef(x_angle,0,0,1); 
	glRotatef(y_angle,0,1,0); 
	glScalef(scale_size,scale_size,scale_size); 

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glBegin(GL_QUADS);

	material_setup();

	glNormal3f(0,0,1);
	glVertex3f(x+0.2,y+0.2,0.0);
	glNormal3f(0,0,1);
	glVertex3f(x+0.8,y+0.2,0.0);
	glNormal3f(0,0,1);
	glVertex3f(x+0.8,y+0.8,0.0);
	glNormal3f(0,0,1);
	glVertex3f(x+0.2,y+0.8,0.0);

	glNormal3f(0,0,1);
	glVertex3f(x+0.2,y+0.2,h);
	glNormal3f(0,0,1);
	glVertex3f(x+0.8,y+0.2,h);
	glNormal3f(0,0,1);
	glVertex3f(x+0.8,y+0.8,h);
	glNormal3f(0,0,1);
	glVertex3f(x+0.2,y+0.8,h);

	glNormal3f(0,-1,0);
	glVertex3f(x+0.2,y+0.2,0.0);
	glNormal3f(0,-1,0);
	glVertex3f(x+0.8,y+0.2,0.0);
	glNormal3f(0,-1,0);
	glVertex3f(x+0.8,y+0.2,h);
	glNormal3f(0,-1,0);
	glVertex3f(x+0.2,y+0.2,h);

	glNormal3f(0,1,0);
	glVertex3f(x+0.2,y+0.8,0.0);
	glNormal3f(0,1,0);
	glVertex3f(x+0.8,y+0.8,0.0);
	glNormal3f(0,1,0);
	glVertex3f(x+0.8,y+0.8,h);
	glNormal3f(0,1,0);
	glVertex3f(x+0.2,y+0.8,h);

	glNormal3f(-1,0,0);
	glVertex3f(x+0.2,y+0.2,0.0);
	glNormal3f(-1,0,0);
	glVertex3f(x+0.2,y+0.8,0.0);
	glNormal3f(-1,0,0);
	glVertex3f(x+0.2,y+0.8,h);
	glNormal3f(-1,0,0);
	glVertex3f(x+0.2,y+0.2,h);

	glNormal3f(1,0,0);
	glVertex3f(x+0.8,y+0.2,0.0);
	glNormal3f(1,0,0);
	glVertex3f(x+0.8,y+0.8,0.0);
	glNormal3f(1,0,0);
	glVertex3f(x+0.8,y+0.8,h);
	glNormal3f(1,0,0);
	glVertex3f(x+0.8,y+0.2,h);

	glEnd();

	glDisable(GL_LIGHTING);

	glPopMatrix();
}

void setup_illumination()
{
	// Intialise and set lighting parameters
	// GLfloat light_pos[] = {0.0,5.0,50.0, 0.0};
	GLfloat light_pos[] = {-10.0,100.0,100.0, 0.0};
	// GLfloat light_ka[] = {0.2, 0.2, 0.2, 1.0};
	// GLfloat light_kd[] = {1.0, 1.0, 0.0, 1.0};
	// GLfloat light_ks[] = {0.0, 0.0, 0.3, 1.0};
	GLfloat light_ka[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat light_kd[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_ks[] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ka);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_kd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_ks);

	// Smooth shading
	glShadeModel(GL_SMOOTH);

	glEnable (GL_LIGHT0);
}

void draw_fluid_surface()
{
	glPushMatrix();

	glEnable (GL_LIGHTING);

	glEnable(GL_NORMALIZE);

	glRotatef(x_angle,0,0,1); 
	glRotatef(y_angle,0,1,0); 
	glScalef(scale_size,scale_size,scale_size); 

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glBegin(GL_TRIANGLES);

	glColor3f(0.0,0.0,0.0);

	material_setup();

	for(int i=0;i<GRID_SIZE-1;i++)
	{
		for(int j=0;j<GRID_SIZE-1;j++)
		{
			glNormal3f(normal_set[i][j].x,normal_set[i][j].y,normal_set[i][j].z);	
			glVertex3f(point_set[i][j].x,point_set[i][j].y,point_set[i][j].z);
			glNormal3f(normal_set[i+1][j].x,normal_set[i+1][j].y,normal_set[i+1][j].z);
			glVertex3f(point_set[i+1][j].x,point_set[i+1][j].y,point_set[i+1][j].z);
			glNormal3f(normal_set[i+1][j+1].x,normal_set[i+1][j+1].y,normal_set[i+1][j+1].z);
			glVertex3f(point_set[i+1][j+1].x,point_set[i+1][j+1].y,point_set[i+1][j+1].z);

			glNormal3f(normal_set[i+1][j+1].x,normal_set[i+1][j+1].y,normal_set[i+1][j+1].z);
			glVertex3f(point_set[i+1][j+1].x,point_set[i+1][j+1].y,point_set[i+1][j+1].z);
			glNormal3f(normal_set[i][j+1].x,normal_set[i][j+1].y,normal_set[i][j+1].z);
			glVertex3f(point_set[i][j+1].x,point_set[i][j+1].y,point_set[i][j+1].z);
			glNormal3f(normal_set[i][j].x,normal_set[i][j].y,normal_set[i][j].z);
			glVertex3f(point_set[i][j].x,point_set[i][j].y,point_set[i][j].z);
		}
	}

	glEnd();

	glDisable(GL_LIGHTING);

	glPopMatrix();
}


void draw_axes()
{
	glPushMatrix();

	glRotatef(x_angle,0,0,1); 
	glRotatef(y_angle,0,1,0); 
	glScalef(scale_size,scale_size,scale_size); 

	glLineWidth(4.5);

	glBegin(GL_LINES);
	
	glColor3f(1.0,0.0,0.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(10.0,0,0);
	
	glColor3f(0.0,0.0,1.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,10.0,0);
	
	glColor3f(0.0,1.0,0.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,0,10.0);	

	glEnd();

	glPopMatrix();
}

void draw_bar_plot()
{
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			draw_single_bar((float)i,(float)j,height_current[i][j]);
		}
	}
}

void generate_point_set()
{
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			glm::vec3 p0((float)i,(float)j,height_current[i][j]);
			point_set[i][j] = p0;
		}
	}
}

void print_heightfield_values()
{
	std::cout.setf(std::ios::fixed, std::ios::floatfield); 
	std::cout.precision(2);
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			std::cout << std::setw(7) << height_current[i][j] << " " ;
		}
		std::cout << "\n";
	}
}

void generate_normals()
{
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			glm::vec3 zerov(0.0,0.0,0.0);
			normal_set[i][j] = zerov;
		}
	}
	for(int i=0;i<GRID_SIZE-1;i++)
	{
		for(int j=0;j<GRID_SIZE-1;j++)
		{
			glm::vec3 a1 = point_set[i][j];
			glm::vec3 b1 = point_set[i+1][j];
			glm::vec3 c1 = point_set[i+1][j+1];

			glm::vec3 a2 = point_set[i+1][j+1];
			glm::vec3 b2 = point_set[i][j+1];
			glm::vec3 c2 = point_set[i][j];

			glm::vec3 cp1 = glm::normalize(glm::cross(b1-a1,c1-a1));
			glm::vec3 cp2 = glm::normalize(glm::cross(b2-a2,c2-a2));

			normal_set[i][j] += cp1;
			normal_set[i+1][j] += cp1;
			normal_set[i+1][j+1] += cp1;

			normal_set[i+1][j+1] += cp2;
			normal_set[i][j+1] += cp2;
			normal_set[i][j] += cp2;

		}
	}
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			normal_set[i][j] = glm::normalize(normal_set[i][j]);
		}
	}
}

void perturb_randomly()
{
	float low = 0.05;
	float high = 0.1;
	int x_r = rand()%GRID_SIZE;
	int y_r = rand()%GRID_SIZE;
	float h_p = low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
	height_current[x_r][y_r] += h_p;
	// height_prev[x_r][y_r] += h_p;
	print_heightfield_values();
	generate_point_set();
	generate_normals();
	glutPostRedisplay();
}

void explicit_solver()
{
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{

			if(i==0)
			{
				if(j==0)
				{
					height_new[i][j] = height_current[i][j] + 
					(height_current[i][j]-height_prev[i][j])*DAMPING + 
					(2.0*height_current[i+1][j] + 2.0*height_current[i][j+1] - 4.0*height_current[i][j])*RATE;
				}
				else if(j==GRID_SIZE-1)
				{
					height_new[i][j] = height_current[i][j] + 
					(height_current[i][j]-height_prev[i][j])*DAMPING + 
					(2.0*height_current[i+1][j] + 2.0*height_current[i][j-1] - 4.0*height_current[i][j])*RATE;
				}
				else
				{
					height_new[i][j] = height_current[i][j] + 
					(height_current[i][j]-height_prev[i][j])*DAMPING + 
					(2.0*height_current[i+1][j] + height_current[i][j-1] + height_current[i][j+1] - 4*height_current[i][j])*RATE;
				}
			}
			else if(i==GRID_SIZE-1)
			{
				if(j==0)
				{
					height_new[i][j] = height_current[i][j] + 
					(height_current[i][j]-height_prev[i][j])*DAMPING + 
					(2.0*height_current[i-1][j] + 2.0*height_current[i][j+1] - 4.0*height_current[i][j])*RATE;
				}
				else if(j==GRID_SIZE-1)
				{
					height_new[i][j] = height_current[i][j] + 
					(height_current[i][j]-height_prev[i][j])*DAMPING + 
					(2.0*height_current[i-1][j] + 2.0*height_current[i][j-1] - 4.0*height_current[i][j])*RATE;
				}
				else
				{
					height_new[i][j] = height_current[i][j] + 
					(height_current[i][j]-height_prev[i][j])*DAMPING + 
					(2.0*height_current[i-1][j] + height_current[i][j-1] + height_current[i][j+1] - 4*height_current[i][j])*RATE;
				}
			}
			else if(j==0)
			{
				height_new[i][j] = height_current[i][j] + 
				(height_current[i][j]-height_prev[i][j])*DAMPING + 
				(height_current[i+1][j] + height_current[i-1][j] + 2.0*height_current[i][j+1] - 4.0*height_current[i][j])*RATE;
			}
			else if(j==GRID_SIZE-1)
			{
				height_new[i][j] = height_current[i][j] + 
				(height_current[i][j]-height_prev[i][j])*DAMPING + 
				(height_current[i+1][j] + height_current[i-1][j] + 2.0*height_current[i][j-1]- 4.0*height_current[i][j])*RATE;
			}
			else
			{
				height_new[i][j] = height_current[i][j] + 
				(height_current[i][j]-height_prev[i][j])*DAMPING + 
				(height_current[i+1][j] + height_current[i-1][j] + height_current[i][j-1] + height_current[i][j+1] - 4.0*height_current[i][j])*RATE;
			}
		}
	}
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			height_prev[i][j] = height_current[i][j];
			height_current[i][j] = height_new[i][j];
		}
	}
	print_heightfield_values();
	generate_point_set();
	generate_normals();
	glutPostRedisplay();
}

void implicit_solver()
{
	int num_points = GRID_SIZE*GRID_SIZE;
	Eigen::SparseMatrix<float> A(num_points,num_points);
	A.reserve(Eigen::VectorXi::Constant(num_points,5));
	Eigen::VectorXf b = Eigen::VectorXf::Zero(num_points);
	Eigen::VectorXf x = Eigen::VectorXf::Zero(num_points);
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			int center = i * GRID_SIZE + j;
			int left_stencil = (i-1) * GRID_SIZE + j;
			int right_stencil = (i+1) * GRID_SIZE + j;
			int top_stencil = i * GRID_SIZE + (j+1);
			int bottom_stencil = i * GRID_SIZE + (j-1);

			//First populate b 
			b(center) = height_current[i][j] + (height_current[i][j]-height_prev[i][j])*DAMPING;
			
			A.insert(center,center)  = (1.0 + 4.0 * RATE);
			
			if(i==0)
			{
				if(j==0)
				{
					A.insert(center,right_stencil) = (-2.0*RATE);
					A.insert(center,top_stencil) = (-2.0*RATE);
				}
				else if(j==GRID_SIZE-1)
				{
					A.insert(center,right_stencil) = (-2.0*RATE);
					A.insert(center,bottom_stencil) = (-2.0*RATE);
				}
				else
				{
					A.insert(center,right_stencil) = (-2.0*RATE);
					A.insert(center,bottom_stencil) = (-1.0*RATE);
					A.insert(center,top_stencil) = (-1.0*RATE);
				}
			}
			else if(i==GRID_SIZE-1)
			{
				if(j==0)
				{
					A.insert(center,left_stencil) = (-2.0*RATE);
					A.insert(center,top_stencil) = (-2.0*RATE);
				}
				else if(j==GRID_SIZE-1)
				{
					A.insert(center,left_stencil) = (-2.0*RATE);
					A.insert(center,bottom_stencil) = (-2.0*RATE);
				}
				else
				{
					A.insert(center,left_stencil) = (-2.0*RATE);
					A.insert(center,bottom_stencil) = (-1.0*RATE);
					A.insert(center,top_stencil) = (-1.0*RATE);
				}
			}
			else if(j==0)
			{
				A.insert(center,top_stencil) = (-2.0*RATE);
				A.insert(center,right_stencil) = (-1.0*RATE);
				A.insert(center,left_stencil) = (-1.0*RATE);
			}
			else if(j==GRID_SIZE-1)
			{
				A.insert(center,bottom_stencil) = (-2.0*RATE);
				A.insert(center,right_stencil) = (-1.0*RATE);
				A.insert(center,left_stencil) = (-1.0*RATE);
			}
			else
			{
				A.insert(center,top_stencil) = (-1.0*RATE);
				A.insert(center,bottom_stencil) = (-1.0*RATE);
				A.insert(center,right_stencil) = (-1.0*RATE);
				A.insert(center,left_stencil) = (-1.0*RATE);
			}
		}
	}

	Eigen::BiCGSTAB<Eigen::SparseMatrix<float> > bicgstab_solver;
	bicgstab_solver.setTolerance(1.0e-4);
	bicgstab_solver.compute(A);
	x = bicgstab_solver.solve(b);
	for(int i=0;i<GRID_SIZE;i++)
	{
		for(int j=0;j<GRID_SIZE;j++)
		{
			height_prev[i][j] = height_current[i][j];
			height_current[i][j] = x(i*GRID_SIZE+j);
		}
	}
	print_heightfield_values();
	generate_point_set();
	generate_normals();
	glutPostRedisplay();
	
}

void display(void)
{
	glEnable(GL_DEPTH_TEST); 
	glClearColor(1,1,1,1); 
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 

	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	gluLookAt(15,13,14,0,0,0,0,0,1);
	draw_axes();
	draw_bar_plot();
	// draw_fluid_surface();
	// draw_text_information();
	glutSwapBuffers();
}

void mouseFunc(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {
    press_x = x; press_y = y; 
    if (button == GLUT_LEFT_BUTTON)
      xform_mode = XFORM_ROTATE; 
	 else if (button == GLUT_RIGHT_BUTTON) 
      xform_mode = XFORM_SCALE; 
  }
  else if (state == GLUT_UP) {
	  xform_mode = XFORM_NONE; 
  }
}

void motionFunc(int x, int y)
{
    if (xform_mode==XFORM_ROTATE) {

      x_angle += (x - press_x)/5.0; 
      if (x_angle > 180) x_angle -= 360; 
      else if (x_angle <-180) x_angle += 360; 
      press_x = x; 
	   
      y_angle += (y - press_y)/5.0; 
      if (y_angle > 180) y_angle -= 360; 
      else if (y_angle <-180) y_angle += 360; 
      press_y = y; 
     }
	else if (xform_mode == XFORM_SCALE){
      float old_size = scale_size;
      scale_size *= (1+ (y - press_y)/60.0); 
      if (scale_size <0) scale_size = old_size; 
      press_y = y; 
    }
    glutPostRedisplay(); 
}

void kbdFunc(unsigned char key,int x,int y)
{
	int cnt=0;
	switch(key)
	{
		case 'q' : case 'Q' :
			exit(0);
			break;	
		case 'r' : case 'R' :
			perturb_randomly(); 
			break;
		case 'e' : case 'E' :
			explicit_solver(); 
			break;
		case 'i' : case 'I' :
			implicit_solver(); 
			break;
	}
}

void reshapeFunc(int h, int w)
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport( tx, ty, tw, th );
	//glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	gluPerspective(60, 1, .1, 100); 
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	//glShadeModel (GL_SMOOTH);
	glutInitWindowSize(800,600);
	int main_window_id = glutCreateWindow("Cloth Simulation - Fast");

	glewExperimental = GL_TRUE;
	glewInit();

	init_fields();
	setup_illumination();
	generate_point_set();

	glutDisplayFunc(display);
	glutMotionFunc(motionFunc);
	glutKeyboardFunc(kbdFunc);
	glutMouseFunc(mouseFunc);
	glutReshapeFunc(reshapeFunc);

	glutMainLoop();

	return 0;
}