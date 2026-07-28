from improutils import camera_calib

camera_matrix, dist_coefs, _ = camera_calib(
    "./data/video/calibration.mp4", (6, 9))

print(camera_matrix)
print(dist_coefs)