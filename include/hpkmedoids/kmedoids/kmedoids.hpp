#pragma once

#include <hpkmedoids/distances.hpp>
#include <hpkmedoids/initializers/initializers.hpp>
#include <hpkmedoids/maximizers/maximizers.hpp>
#include <string>

namespace hpkmedoids
{
template <typename T, Parallelism Level = Parallelism::Serial, class DistanceFunc = L1Norm<T>>
class KMedoids
{
public:
    KMedoids(const std::string& initializer, const std::string& maximizer) :
        p_initializer(createInitializer<T, Level>(initializer)), p_maximizer(createMaximizer<T, Level>(maximizer))
    {
    }

    virtual ~KMedoids() = default;

    const Clusters<T>* const fit(const Matrix<T>* const data, const int& numClusters, const int& numRepeats)
    {
        auto distMat = DistanceMatrix<T>::template create<Level, DistanceFunc>(data, numClusters);

        for (int i = 0; i < numRepeats; ++i)
        {
            Clusters<T> clusters(data, &distMat);
            p_initializer->initialize(data, &clusters, &distMat);
            p_maximizer->maximize(data, &clusters, &distMat);
            compareResults(clusters, m_bestClusters);
        }

        return getResults();
    }

    const Clusters<T>* const getResults() const { return &m_bestClusters; }

    virtual void reset() { m_bestClusters = Clusters<T>(); }

protected:
    void compareResults(const Clusters<T>& candidateClusters, Clusters<T>& bestClusters)
    {
        if (candidateClusters < bestClusters)
            bestClusters = std::move(candidateClusters);
    }

protected:
    Clusters<T> m_bestClusters;

private:
    std::unique_ptr<IInitializer<T>> p_initializer;
    std::unique_ptr<IMaximizer<T>> p_maximizer;
};
}  // namespace hpkmedoids