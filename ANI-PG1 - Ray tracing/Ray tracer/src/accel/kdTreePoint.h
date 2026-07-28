#pragma once

/*
    kdTreePoint.h
    KD-tree point cloud helper for spatial partitioning of 3D points.
    Includes recursive build and nearest-point search support.
*/

#include <vector>
#include <algorithm>

#include "../math/vec3.h"
#include "../core/triangle.h"


enum class eAxis : uint8_t { 
    X = 0, 
    Y = 1, 
    Z = 2, 
    None = 3 // leaf node indicator
};

struct sKDTreeNode{
   //  sKDTreeNode* left = nullptr;
   //  sKDTreeNode* right = nullptr;
   vec3 location;
   eAxis axis = eAxis::None;
   uint32_t leftIdx = -1; // index of left child in array
   uint32_t rightIdx = -1; // index of right child in array

   bool isLeaf() const { return leftIdx == -1 && rightIdx == -1; }
};

class cKdTreePoint {
public:

   cKdTreePoint(std::vector<vec3>& points){
      treeNodes.reserve(points.size());
      buildRecursive(points, 0, points.size(), 0);
      treeNodes.shrink_to_fit();
      //root = buildIterative(points);
   }

   // ~cKdTreePoint(){
   //    delSubtree(root);
   // }

   // cKdTreePoint(const cKdTreePoint&) = delete;
   // cKdTreePoint& operator=(const cKdTreePoint&) = delete;

   // void delSubtree(sKDTreeNode* node){
   //    if(node){
   //       delSubtree(node->left);
   //       delSubtree(node->right);
   //       delete node;
   //    }
   // }

   // O(nlogn)
   uint32_t buildRecursive(std::vector<vec3>& points, uint32_t start, uint32_t end, uint32_t depth){

      if(start >= end){
         return -1;
      }

      uint32_t axisIdx = depth % 3;
      eAxis axis = static_cast<eAxis>(axisIdx);

      uint32_t medianIdx = start + (end - start) / 2;
      auto cmp = [&axis](const vec3& a, const vec3& b) {
         switch(axis){
            case eAxis::X: return a.x < b.x;
            case eAxis::Y: return a.y < b.y;
            case eAxis::Z: return a.z < b.z;
            default: return false;
         }
      };

      std::nth_element(points.begin() + start, points.begin() + medianIdx, points.begin() + end, cmp);

      uint32_t idx = treeNodes.size();
      treeNodes.emplace_back();
      treeNodes[idx].location = points[medianIdx];
      treeNodes[idx].axis = axis;

      treeNodes[idx].leftIdx = buildRecursive(points, start, medianIdx, depth + 1);
      treeNodes[idx].rightIdx = buildRecursive(points, medianIdx + 1, end, depth + 1);

      return idx;
   }

   // O(n^2)
   uint32_t buildIterative(std::vector<vec3>& points){
      uint32_t node = -1;
      for(uint32_t i = 0; i < points.size(); ++i){
         node = insert(node, points[i], 0);
      }
      return node;
   }

   uint32_t insert(uint32_t nodeIdx, const vec3& point, uint32_t depth){
      if(nodeIdx == -1){
         sKDTreeNode* newNode;
         newNode->axis = static_cast<eAxis>(depth % 3);
         newNode->location = point;
         newNode->leftIdx = -1;
         newNode->rightIdx = -1;
         uint32_t idx = treeNodes.size();
         treeNodes.push_back(*newNode);
         return idx;
      }

      eAxis axis = treeNodes[nodeIdx].axis;
      if((axis == eAxis::X && point.x < treeNodes[nodeIdx].location.x) ||
         (axis == eAxis::Y && point.y < treeNodes[nodeIdx].location.y) ||
         (axis == eAxis::Z && point.z < treeNodes[nodeIdx].location.z)){
         treeNodes[nodeIdx].leftIdx = insert(treeNodes[nodeIdx].leftIdx, point, depth + 1);
      } else {
         treeNodes[nodeIdx].rightIdx = insert(treeNodes[nodeIdx].rightIdx, point, depth + 1);
      }
      return nodeIdx;
   }

   bool find(uint32_t nodeIdx, const vec3& point) const{
      if(nodeIdx == -1){
         return false;
      }
      if(treeNodes[nodeIdx].location == point){
         return true;
      }

      eAxis axis = treeNodes[nodeIdx].axis;
      if((axis == eAxis::X && point.x < treeNodes[nodeIdx].location.x) ||
         (axis == eAxis::Y && point.y < treeNodes[nodeIdx].location.y) ||
         (axis == eAxis::Z && point.z < treeNodes[nodeIdx].location.z)){
         return find(treeNodes[nodeIdx].leftIdx, point);
      } else {
         return find(treeNodes[nodeIdx].rightIdx, point);
      }
   }

private:
   //sKDTreeNode* root;
   std::vector<sKDTreeNode> treeNodes;
};
