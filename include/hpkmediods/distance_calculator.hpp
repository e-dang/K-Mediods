#pragma once

#include <hpkmediods/types/parallelism.hpp>
#include <matrix/matrix.hpp>
#include <type_traits>

namespace hpkmediods
{
template <typename T, Parallelism Level, class DistanceFunc>
class DistanceCalculator
{
public:
    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI, Matrix<T>> calculateDistanceMatrix(
      const Matrix<T>* const mat1, const Matrix<T>* const mat2) const
    {
        Matrix<T> distanceMat(mat1->rows(), mat2->rows(), true, std::numeric_limits<T>::max());

        for (int i = 0; i < mat1->rows(); ++i)
        {
            for (int j = 0; j < mat2->numRows(); ++j)
            {
                distanceMat.at(i, j) =
                  m_distanceFunc(mat1->crowBegin(i), mat1->crowEnd(i), mat2->crowBegin(j), mat2->crowEnd(j));
            }
        }

        return distanceMat;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid, Matrix<T>> calculateDistanceMatrix(
      const Matrix<T>* const mat1, const Matrix<T>* const mat2) const
    {
        Matrix<T> distanceMat(mat1->rows(), mat2->rows(), true, std::numeric_limits<T>::max());

#pragma omp parallel for shared(mat1, mat2, distanceMat), schedule(static)
        for (int i = 0; i < mat1->rows(); ++i)
        {
            for (int j = 0; j < mat2->numRows(); ++j)
            {
                distanceMat.at(i, j) =
                  m_distanceFunc(mat1->crowBegin(i), mat1->crowEnd(i), mat2->crowBegin(j), mat2->crowEnd(j));
            }
        }

        return distanceMat;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI> calculateDistanceMatrix(
      const Matrix<T>* const mat1, const Matrix<T>* const mat2, Matrix<T>* const distanceMat) const
    {
        for (int i = 0; i < mat1->rows(); ++i)
        {
            for (int j = 0; j < mat2->numRows(); ++j)
            {
                distanceMat->at(i, j) =
                  m_distanceFunc(mat1->crowBegin(i), mat1->crowEnd(i), mat2->crowBegin(j), mat2->crowEnd(j));
            }
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid> calculateDistanceMatrix(
      const Matrix<T>* const mat1, const Matrix<T>* const mat2, Matrix<T>* const distanceMat) const
    {
#pragma omp parallel for shared(mat1, mat2, distanceMat), schedule(static)
        for (int i = 0; i < mat1->rows(); ++i)
        {
            for (int j = 0; j < mat2->numRows(); ++j)
            {
                distanceMat->at(i, j) =
                  m_distanceFunc(mat1->crowBegin(i), mat1->crowEnd(i), mat2->crowBegin(j), mat2->crowEnd(j));
            }
        }
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::Serial || _Level == Parallelism::MPI, std::vector<T>> calculateDistanceSums(
      const Matrix<T>* const distanceMat) const
    {
        std::vector<T> distanceSums(distanceMat->rows());
        for (int i = 0; i < distanceMat->rows(); ++i)
        {
            distanceSums[i] = std::accumulate(distanceMat->crowBegin(i), distanceMat->crowEnd(i), 0.0);
        }

        return distanceSums;
    }

    template <Parallelism _Level = Level>
    std::enable_if_t<_Level == Parallelism::OMP || _Level == Parallelism::Hybrid, std::vector<T>> calculateDistanceSums(
      const Matrix<T>* const distanceMat) const
    {
        std::vector<T> distanceSums(distanceMat->rows());

#pragma omp parallel for shared(distanceMat, distanceSums), schedule(static)
        for (int i = 0; i < distanceMat->rows(); ++i)
        {
            distanceSums[i] = std::accumulate(distanceMat->crowBegin(i), distanceMat->crowEnd(i), 0.0);
        }

        return distanceSums;
    }

private:
    DistanceFunc m_distanceFunc;
};
}  // namespace hpkmediods