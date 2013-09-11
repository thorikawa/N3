#ifndef MARKER_MANAGER_H_
#define MARKER_MANAGER_H_

#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <fstream>
#include <vector>

using namespace std;

namespace N3 {
    class MarkerManager {
    public:
        static void start(string fileName);
    private:
        static void mouseHandler(int event, int x, int y, int flags, void* param);
        static const int LINE_WIDTH = 1;
    };
}

#endif /* MARKER_MANAGER_H_ */
