#include<iostream>
#include<cmath>
/** 
    This class represents 
    a coulomb sphere inside
    an electromagnetic field
*/
#define PI 3.14 
#define MU 4*PI
#define E_O 0.005
extern float t0;

class sphere{
    
    float Q; 
    float q_coulomb_wire;
    unsigned short int lambda;
    float mass;

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 Lorentz_Force;
    glm::vec3 B_field;
    glm::vec3 E_field;

    public:
        sphere();
        
        void set_velocity();
        void calculate_B(float I, float dt);
        void calculate_E(float I, float dt);

        void calculate_Lorentz_Force( float dt);
        void calculate_motion_equations(float dt);
        void reset();

        glm::vec3 get_B_field();
        glm::vec3 get_E_field();
        glm::vec3 get_Force();
        glm::vec3 get_position();
        glm::vec3 get_velocity();
        glm::vec3 get_acceleration();

};


sphere::sphere(){
    position  = glm::vec3(0.0f,1.0f,-10.0f);
    Q = 1.6; //e-19
    mass = 0.9; // e-31
    E_field = glm::vec3(0.0f,0.0f,0.0f);
    q_coulomb_wire = 0;
    lambda = 10;
    
    // Setup the environment
    set_velocity();

}

void sphere::reset(){
    position      = glm::vec3(0.0f,1.0f,-10.0f);
    E_field       = glm::vec3(0.0f,0.0f,0.0f);
    B_field       = glm::vec3(0.0f,0.0f,0.0f);
    Lorentz_Force = glm::vec3(0.0f,0.0f,0.0f);
    q_coulomb_wire = 0;

    set_velocity();

}


void sphere::set_velocity(){
    velocity = glm::vec3(0.0f,0.0f,0.0f);
}


void sphere::calculate_E(float I,float dt){
    q_coulomb_wire = I*dt + q_coulomb_wire ;
    float angle;

    if(position.x == 0)
        angle == PI;
    else
        angle = atan(abs(position.z/position.x));
    float k = (1/(2*PI*E_O));
    float r = sqrt(pow(position.x,2) + pow(position.z,2));

    float E_meter = -k*q_coulomb_wire/(r*sqrt(pow(r,2) + pow(lambda,2)/4)  );
   

    //r_unit in cylindrical coords is equal : r = [x/r, y/r, 0]
    E_field = glm::vec3(E_meter*cos(angle),
                        0.0,
                        E_meter*sin(angle));

}



void sphere::calculate_B(float I, float dt){
    float angle;
    
    if(position.x == 0)
        angle == PI;
    else
        angle = atan(abs(position.z/position.x));

    float B_meter = (MU/(2*PI))*I*2/( sqrt(pow(position.x,2) + pow(position.z,2)) );
    //std::cout <<  B_meter << std::endl;
    B_field = glm::vec3(-B_meter*sin(angle),
                        0.0f,
                        B_meter*cos(angle));
    
    //Calculate Electric field.
    calculate_E(I,dt);
    //Calculate now Lorentz force.
    calculate_Lorentz_Force(dt);
}

void sphere::calculate_Lorentz_Force(float dt){
    calculate_motion_equations(dt);
    Lorentz_Force = E_field + glm::cross(velocity,B_field);
    Lorentz_Force = glm::vec3(Lorentz_Force.x*Q,Lorentz_Force.y*Q,Lorentz_Force.z*Q);    
}

void sphere::calculate_motion_equations(float dt){
    
    
    acceleration = glm::vec3(Lorentz_Force.x*mass,Lorentz_Force.y*mass,Lorentz_Force.z*mass);

    velocity = glm::vec3(acceleration.x*dt + velocity.x,
                         acceleration.y*dt + velocity.y,
                         acceleration.z*dt + velocity.z) ;
    
    position = glm::vec3(position.x + (velocity.x*dt), 
                         position.y + (velocity.y*dt),
                         position.z + (velocity.z*dt)); // So now I know my new position :) 

}

glm::vec3 sphere::get_Force(){
    return Lorentz_Force;
}

glm::vec3 sphere::get_B_field(){
    return B_field;
}

glm::vec3 sphere::get_E_field(){
    return E_field;
}


glm::vec3 sphere::get_position(){
    return position;
}

glm::vec3 sphere::get_velocity(){
    return velocity;
}

glm::vec3 sphere::get_acceleration(){
    return acceleration;
}