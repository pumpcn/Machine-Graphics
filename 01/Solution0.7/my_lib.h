#ifndef MY_LIB
#define MY_LIB

#define pi 3.141592654
#define background Vec3f( 0.2, 0.7, 0.8 )
#define MAX_DEPTH 200 
#include "geometry.h"

class Light{
public:
	Light( const Vec3f& p, const float& i ); 
	Vec3f position;	 // Центр источника света.
	float intensity; // Интенсивность источника.
};

class Material{
public:	
	Material( const Vec3f& color, 
		  const float bg, 
		  const float df, 
		  const float sc, 
		  const bool flag );
	Material();
	float backg_coef;     // Коэффициент фонового    освещения.
	float spec_coef;      // Коэффициент зеркального освещения.
	float diff_coef;      // Коэффициент диффузного  освещения.
	bool mirror;
	Vec3f color;
	Material operator*( const float number ) const;
	float operator*( const Material& right ) const;
};

class Sphere{
public:
	Vec3f center;
        float radius;      
	Material material; 

	// Инициализируем центр сферы, ее радиус и материал.
        Sphere( const Vec3f &c, const float &r, const Material& m );
	
       	// Пересекает ли сферу луч, исходящий из orig, по направлению dir, если true - расстояние до нее, иначе - -1.
        float ray_intersect( const Vec3f& orig, const Vec3f& dir ) const;
};

// Пересекает ли наш луч какой либо из объектов?(true/false) Если да, то присвоить this_material материал этого объекта.
//bool scene_intersect( const Vec3f&, Vec3f&, std::vector <Sphere>, Material& this_material ); // Почему работает?!?
Vec3f cast_ray( const Vec3f& orig, const Vec3f& dir, const std::vector <Sphere>& spheres, const std::vector <Light>& lights, size_t am_rec );


//Vec3f cast_ray( const Vec3f&, const Vec3f&, std::vector <Sphere> );			       // Почему работает?!?
void render( const std::vector<Sphere>&, const std::vector<Light>& );

#endif
