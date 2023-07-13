#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <chrono>

using namespace cv;


int main() {
    // Video dosyasını yükle
    cv::VideoCapture cap("v4l2src device=/dev/video1 ! image/jpeg, framerate=30/1,width=640,height=480,format=(string)MJPG ! jpegdec ! videoconvert ! appsink");
    if (!cap.isOpened()) {
        std::cout << "Video dosyası yüklenemedi!" << std::endl;
        return -1;
    }

    // İlk kareyi oku
    cv::Mat frame;
    cap >> frame;
    cv::namedWindow("Nesne Takibi");

    // İzlenecek nesneyi seçin
    cv::Rect boundingBox = cv::selectROI("Nesne Takibi",frame, false);

    // CSRT takipçi nesnesini oluştur
    Ptr<Tracker> tracker = TrackerCSRT::create();

    // Nesneyi takip etmek için başlangıç noktasını ve boyutunu ayarla
    tracker->init(frame, boundingBox);

    // Videoyu oynat ve nesneyi takip et
    while (cap.read(frame)) {
        // Başlangıç zamanını kaydet
        auto start = std::chrono::high_resolution_clock::now();

        // Nesneyi güncelle
        bool success = tracker->update(frame, boundingBox);

        // Bitiş zamanını kaydet
        auto end = std::chrono::high_resolution_clock::now();
        
        // Süreyi hesapla
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (success) {
            // Nesneyi çerçeve üzerinde göster
            cv::rectangle(frame, boundingBox, cv::Scalar(255, 0, 0), 2, 1);
        }

        // Çerçeveyi göster
        cv::imshow("Nesne Takibi", frame);

        // Süreyi yazdır
        std::cout << "Takip süresi: " << duration.count() << " ms" << std::endl;

        // 'q' tuşuna basıldığında döngüden çık
        if (cv::waitKey(1) == 'q')
            break;
    }

    // Pencereyi kapat
    cv::destroyAllWindows();

    return 0;
}
