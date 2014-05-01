import cv2
import numpy as np
import mahotas as mh



def main():
    cap = cv2.VideoCapture(0)
    prev_grey_frame = None
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        grey = np.uint8(np.mean(frame, axis=2))

        ch = 0xFF & cv2.waitKey(5)
        if ch == 27:
            break
        if prev_grey_frame is not None:
            flow = cv2.calcOpticalFlowFarneback(prev_grey_frame, grey,
                    pyr_scale=0.5, levels=3, winsize=15,
                    iterations=3, poly_n=5, poly_sigma=1.2, flags=0)
            mag_flow = np.uint8(np.sum(np.abs(5 * flow), axis=2))
            mask_flow = np.uint8(255 * (mag_flow > 50))
            mask_flow = cv2.dilate(mask_flow,
                            cv2.getStructuringElement(cv2.MORPH_RECT,(15,15)))
            vis_frame = frame.copy()

            fx, fy = flow[:, :, 0], flow[:, :, 1]

            for contour in cv2.findContours(mask_flow,
                                            cv2.cv.CV_RETR_EXTERNAL,
                                            cv2.cv.CV_CHAIN_APPROX_SIMPLE)[0]:
                rect = cv2.minAreaRect(contour)
                center, size, _ = rect
                if np.min(size) < 100:
                    continue

                cur_mask = np.zeros(grey.shape)
                cv2.drawContours(cur_mask, [contour], 0, 255, -1)
                mean_fx = np.mean(fx[np.bool8(cur_mask)])
                mean_fy = np.mean(fy[np.bool8(cur_mask)])
                p2 = (int(center[0] + mean_fx * 10),
                      int(center[1] + mean_fy * 10))
                cv2.line(vis_frame, (int(center[0]), int(center[1])),
                         p2, (0, 255, 0))

                box = cv2.cv.BoxPoints(rect)
                box = np.int0(box)
                for i in xrange(len(box)):
                    cv2.line(vis_frame, tuple(box[i - 1]),
                             tuple(box[i]), (0, 0, 255), 2)



            cv2.imshow('mag_flow', vis_frame)


        prev_grey_frame = grey.copy()





if __name__ == '__main__':
    main()
