import sensor, image, time, math
from pyb import Servo, Pin

tilt_pin = Pin('P1', Pin.OUT_PP)
pan_servo = Servo(3)
pan_servo.calibration(500, 2500, 500)
pan_angle = 95
tilt_angle = 64
pan_servo.angle(pan_angle)
def set_tilt_angle(angle):
    angle = max(0, min(180, angle))
    pulse_width = 500 + int(angle * 2000 // 180)
    tilt_pin.high()
    time.sleep_us(pulse_width)
    tilt_pin.low()
    time.sleep_us(20000 - pulse_width)
set_tilt_angle(tilt_angle)
time.sleep_ms(1000)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False)
sensor.set_contrast(0)
sensor.set_brightness(0)

rect_threshold = 15000
failure_count = 0
is_searching = False
search_start_angle = pan_angle
search_progress = 0

def is_rectangle(corners, angle_tol=25):
    for i in range(4):
        p1 = corners[i - 1]
        p2 = corners[i]
        p3 = corners[(i + 1) % 4]
        v1 = (p1[0]-p2[0], p1[1]-p2[1])
        v2 = (p3[0]-p2[0], p3[1]-p2[1])
        dot = v1[0]*v2[0] + v1[1]*v2[1]
        mag1 = math.sqrt(v1[0]**2 + v1[1]**2)
        mag2 = math.sqrt(v2[0]**2 + v2[1]**2)
        if mag1 * mag2 < 0.001: return False
        cos_angle = dot / (mag1 * mag2)
        cos_angle = max(-1, min(1, cos_angle))
        angle = math.degrees(math.acos(cos_angle))
        if abs(angle - 90) > angle_tol: return False
    return True

while True:
    img = sensor.snapshot()
    target = None
    rects = img.find_rects(threshold=rect_threshold)
    if rects:
        for r in rects:
            if is_rectangle(r.corners()):
                target = r
                break
    if target:
        failure_count = 0
        is_searching = False
        corners = target.corners()
        # 对角线(绿色) + 框(红色)
        img.draw_line(corners[0][0],corners[0][1], corners[2][0],corners[2][1], (0,255,0))
        img.draw_line(corners[1][0],corners[1][1], corners[3][0],corners[3][1], (0,255,0))
        for i in range(4):
            img.draw_line(corners[i][0],corners[i][1], corners[(i+1)%4][0],corners[(i+1)%4][1], (255,0,0))
        # 原版稳定算法求中心
        tx = sum(pt[0] for pt in corners) // 4
        ty = sum(pt[1] for pt in corners) // 4
        img.draw_line(tx-4,ty,tx+4,ty,(0,0,255))
        img.draw_line(tx,ty-4,tx,ty+4,(0,0,255))
        cx, cy = img.width()//2, img.height()//2
        dx, dy = tx - cx, ty - cy
        pan_angle -= dx * 0.1
        tilt_angle += dy * 0.1
        pan_angle = max(min(pan_angle, 180), 0)
        tilt_angle = max(min(tilt_angle, 180), 0)
        pan_servo.angle(pan_angle)
        set_tilt_angle(tilt_angle)
    else:
        failure_count += 1
        if not is_searching and failure_count >= 30:
            is_searching = True
            search_start_angle = pan_angle
            search_progress = 0
        if is_searching:
            search_progress += 3
            pan_angle = search_start_angle + search_progress
            if pan_angle > 180: pan_angle -= 180
            elif pan_angle < 0: pan_angle += 180
            pan_servo.angle(pan_angle)
            if search_progress >= 360:
                is_searching = False
                failure_count = 0
    time.sleep_ms(50)
