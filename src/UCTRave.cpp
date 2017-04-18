#include <cmath>

#include "UCTRave.h"

using std::sqrt;

double score(const UCTParameters &params,
             bool optimistic,
             int nparent,
             int nchild, double qchild,
             int nidchild, int qidchild) {
    if (optimistic && nchild == 0) {
	return 1e8;
    }
    if (nchild == 0) {
        return -1e8;
    }

    double beta;

    switch (params.schedule) {

    case HAND_SELECTED:
        beta = sqrt(params.k/(3.0*nparent+params.k));
        break;

    case MINIMUM_MSE:
        beta = nidchild/(nchild+nidchild+
                         4.0*params.bias*params.bias*nchild*nidchild);
        break;
    }

#if 0
    // XXX
    beta = 0.0;
#endif

    double s = qchild/nchild;
    double sid = nidchild ? qidchild/nidchild : 0.0;
    double mix = (1-beta)*s+beta*sid;

    return mix+(optimistic ? params.c : 0)*sqrt(2.0*log(nparent)/nchild);
}
