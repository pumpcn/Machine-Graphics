#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

#include "geometry.h" // Кастомная библиотека для работы с геометрическими объектами посредством законов алгебры.
#include "my_lib.h"   // Основная библ-ка, в кот-ой нах-ся описания и справки всех функций.

bool scene_intersect( Vec3f orig, // Вернуть const и &!!!!!!!!!!
	              Vec3f dir, // Вернуть const и &!!!!!!!!!!!
		      std::vector <Sphere>& spheres, 
		      std::vector <Light>& lights,
		      Material& this_material ){ // Вернуть const перед vector<...>!

	float dist;          // Расстояние до текущего объекта.
	float min_dist = -1; // Расстояние до ближайшего объекта.
	int min_index  = -1; // Индекс ближайшего объекта.
	for( int i = 0; i < spheres.size(); ++i ){

		dist = spheres[i].ray_intersect( orig, dir );
		if( min_index == -1 && dist != -1 ){		
			min_dist = dist;
			min_index = i;
		}
		else
		if( dist != -1 )
			if( min_dist > dist ){
				min_dist = dist;
				min_index = i;
			}
	}

	if( min_index != -1 ){

		float all_cur_point_intensity = 0;
		this_material = spheres[min_index].material;
		for( int i = 0; i < lights.size(); ++i ){

			Vec3f cur_point = orig + dir * min_dist; // Р.вектор точки, в кот. считаем цвет.
			Vec3f cur_light_dir = cur_point - lights[i].position;
			float cur_intensity_koef = sqrt(cur_light_dir * cur_light_dir);// Сферы дальше - света меньше.
			Vec3f sphere_normal = cur_point - spheres[min_index].center;
			cur_light_dir.normalize();
			sphere_normal.normalize();
			float cur_point_intensity = -(cur_light_dir * sphere_normal);
			all_cur_point_intensity += cur_point_intensity * lights[i].intensity / cur_intensity_koef;

		}

		this_material.diffuse_color = this_material.diffuse_color * all_cur_point_intensity;
		return true;
	}
	return false;
}

Vec3f cast_ray( const Vec3f& orig, 
		const Vec3f& dir, 
		std::vector <Sphere> spheres, 
		std::vector <Light> lights ){ // Поставить const!!!!! и &!!!!!

	Material this_material;
	// Бросаем луч, пишем в пиксель цвет ближайшего объекта с учетом угла и интенсивности света от источников.
	if( scene_intersect( orig, dir, spheres, lights, this_material ) )
		return this_material.diffuse_color;
	return Vec3f( 0.2, 0.7, 0.8 ); // Фон.
}

// Рисуем композицию - если луч пересекает объект, то ставим соотв. цвету объекта пиксель с учетом падения света.
void render( const std::vector <Sphere>& objects, const std::vector <Light>& lights ){

	const int width    = 1024;
	const int height   = 768;
    	std::vector<Vec3f> framebuffer( width * height );

    	for ( size_t j = 0; j < height; j++ )
   	 	for ( size_t i = 0; i < width; i++ ){
			
			float x = (width/(float)height)*(i -  float(width)/2)/float(width);
			float y = -(j - float(height)/2)/float(height);
			Vec3f dir = Vec3f(x,y, 1); // +x = направо, +y = вверх, +z = вдаль.
			dir.normalize();            // Приводим к длину вектора dir к равной 1, сохраняя направление.
            		framebuffer[i+j*width] = cast_ray( Vec3f(0,0,0), dir, objects, lights );
		}
    
    	std::ofstream ofs; // save the framebuffer to file.
    	ofs.open("./out.ppm");
    	ofs << "P6\n" << width << " " << height << "\n255\n";
    	for ( size_t i = 0; i < height*width; ++i ) 
		for( size_t j = 0; j < 3; ++j )
			ofs << (char)( 255 * std::max( 0.f, std::min(1.f, framebuffer[i][j] )) );
    ofs.close();
}



//----------------------------------------Light:-------------------------------------------------------
Light::Light( const Vec3f& p, const float& i ) : position(p), intensity(i) {}



//----------------------------------------Material:-----------------------------------------------------
Material::Material( const Vec3f& color ) : diffuse_color(color) {}
Material::Material(){}

Material Material::operator*( const float number ){   // Умножения материала (пока только цвета) на число.
	Material tmp;
	tmp.diffuse_color = this->diffuse_color * number;
	return tmp;
}

float Material::operator*( const Material& right ){   // Умножение материала (пока только цвета) на материал (скалярно).
	float scalar;
	scalar = this->diffuse_color * right.diffuse_color;
	return scalar;
}

//----------------------------------------Sphere:-------------------------------------------------------
Sphere::Sphere( const Vec3f& c, const float& r, const Material& m ): center(c), radius(r), material(m) {}

// >0 - растояние до пересечения, -1 - не пересекает. (подробней см. my_lib.h)
float Sphere::ray_intersect( const Vec3f& orig, const Vec3f& dir ){

        Vec3f L = center - orig;
	
        float cosFI = L*dir / (sqrt(L*L) * sqrt(dir*dir));// cos(dir,радиус-вектор ц/c).
	float r2 = radius*radius;
        float d2 = L*L*(1 - cosFI*cosFI);
        if( d2 > r2 )
                return -1; // Расстояние до ПРЯМОЙ больше радуса.
        if( cosFI < 0 )
                return -1; // 'Смотрят' в разные полупространства (стороны).
        float dist_to_center2 = center*center; // Квадрат расстояния от источника света до центра сферы.
	float dist_d = sqrt(dist_to_center2 + d2); // 'почти' Расстояние до сферы.
	float d = sqrt(r2 - d2); // То самое 'почти'.
	return dist_d - d;
}

