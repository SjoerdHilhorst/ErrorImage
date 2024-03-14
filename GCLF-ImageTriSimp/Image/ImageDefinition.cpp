#include "ImageDefinition.h"
#include <iostream>

namespace GCLF
{
namespace ImageTriSimp
{
using namespace Utils;

void ImageT::resize(int w, int h)
{
  width = w;
  height = h;
  pixel_colors.resize(h, w);
}

/// @brief reset mask to zero.
/// @param mask An integer matrix that has the same shape with image.
void ImageT::reset_mask(Eigen::MatrixXi& mask)
{
  if (mask.rows() != height || mask.cols() != width)
    mask.resize(height, width);

  mask.setZero();
}

ImageT::Color FaceColor::get_color(double x, double y)const
{
  switch (type)
  {
  case Type::Constant:
    return cc;
    break;
  case Type::Linear:
    return lc_x * x + lc_y * y + cc;
    break;
  case Type::Quadratic:
    return qc_xx * (x * x) + qc_xy * (x * y) + qc_yy * (y * y)
      + lc_x * x + lc_y * y + cc;
    break;
  case Type::Mesh:
    return get_mesh_color(x, y);
  default:
    return cc;
    break;
  }
}

ImageT::Color FaceColor::get_constant_color()const
{
  return cc;
}

ImageT::Color FaceColor::get_linear_color(double x, double y)const
{
  return lc_x * x + lc_y * y + cc;
}

ImageT::Color FaceColor::get_quadratic_color(double x, double y)const
{
  return qc_xx * (x * x) + qc_xy * (x * y) + qc_yy * (y * y)
    + lc_x * x + lc_y * y + cc;
}

ImageT::Color FaceColor::get_mesh_color(double x, double y) const
{
  float u, v, w;
  auto [index, bary_coords] = findContainingTriangle(x, y);

  if (index == -1) {
      Logger::dev_logger->warn("could not find triangle for pixel {} {} ", x, y);

      return ImageT::Color(255, 0, 0);
  }

  auto &tri = triangles[index];
  // return tri.color0 * bary_coords.x() + tri.color1 * bary_coords.z() + tri.color2 * bary_coords.y();
  return (tri.color0 + tri.color1 + tri.color2) / 3;
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
Vec3d FaceColor::Barycentric(Vec3d p, Vec3d a, Vec3d b, Vec3d c) const
{
    Vec3d v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return Vec3d(u, w, v);
}

std::pair<size_t, Vec3d> FaceColor::containsPoint(MeshTriangle t, Vec3d p) const
{
  auto bary_coords = Barycentric(p, t.ver0, t.ver1, t.ver2);
  float eps = 1e-5;
  if ((bary_coords.x() >= 0 - eps) && (bary_coords.x() <= 1 + eps)  && (bary_coords.y() >= 0 - eps) && (bary_coords.y() <= 1 + eps) && (bary_coords.z() >= 0 - eps) && (bary_coords.z() <= 1 + eps)) {
      return {1, bary_coords}; 
  } 
  
  // Logger::dev_logger->warn("Barycentric coords not in triangle {} {} {}", bary_coords.x(), bary_coords.y(), bary_coords.z());
  return {-1, bary_coords};
  
}

std::pair<int, Vec3d> FaceColor::findContainingTriangle(double x, double y) const
{
  Vec3d bary_coords; 
  for(size_t i =0; i < triangles.size(); ++i)
  {
    Vec2d p0(triangles[i].ver0.x(), triangles[i].ver0.y());
    Vec2d p1(triangles[i].ver1.x(), triangles[i].ver1.y());
    Vec2d p2(triangles[i].ver2.x(), triangles[i].ver2.y());

    auto [hasPoint, bary_coords] = containsPoint(triangles[i], Vec3d(x, y, 0));

    if (hasPoint == 1) {
      return {i, bary_coords};
    }
  }
  return {-1, bary_coords};
}



}// namespace ImageTriSimp
}// namespace GCLF