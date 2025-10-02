import game.main as m
import pygame
import pytest
import game.Classes as c
import game.Classes.constants as constants
from game.Classes import paddle, ball, bricks, laser


def test_ball_brick_collide():
    expected = (200, 15, True)
    expected1 = (200, 10, False)
    result = m.ball_brick_collision(100, 10, False, None)
    assert expected == result or expected1 == result

    result1 = m.ball_brick_collision(0, 0, True, None)
    expected2 = (100, 0, True)
    expected3 = (10, 5, True)
    assert expected2 == result1 or expected3 == result1


padddle = m.Paddle()
lasser = m.Laser(padddle.paddle_x, padddle.paddle_y)
balls = m.Ball()
all_sprites = pygame.sprite.Group()
all_sprites.add(padddle, balls)
briccks = m.Bricks(all_sprites)


def test_paddle():
    padddle.reset()
    assert padddle is not None
    assert padddle.direction == 0
    assert padddle.paddle_x == constants.paddle_x_pos
    assert padddle.paddle_y == constants.paddle_y_pos
    assert padddle.rect.center == (padddle.paddle_x, padddle.paddle_y)

    for i in range(10):
        padddle.left()
    assert padddle.direction == -1
    assert padddle.paddle_x == constants.paddle_x_pos - 100

    padddle.reset()
    assert padddle.rect.center == (constants.paddle_x_pos, constants.paddle_y_pos)

    for i in range(10):
        padddle.right()
    assert padddle.direction == 1
    assert padddle.paddle_x == constants.paddle_x_pos + 100


def test_laser():
    assert lasser.laser_x == constants.paddle_x_pos

    for _ in range(30):
        lasser.update(True)
    assert True == lasser.check_collision(briccks.all_bricks)
    i = 30
    for _ in range(i):
        lasser.update(True)
        if i == 29:
            assert False == lasser.update(True)


def test_ball():
    balls.reset()
    padddle.reset()
    assert balls.rect.center == (constants.ball_x_pos, constants.ball_y_pos)
    padddle.direction=0
    for i in range(50):
        balls.update()
        if True==   balls.check_collision(padddle):
            print("collided")
            break
    for i in range(500):
        balls.update()
        padddle.left()
        if balls.gonna_lose() == True:
            print("lost")
            break
test_ball()

def test_bricks():
    counter=0
    for row in range(constants.brick_row):
        for col in range(constants.brick_cols):
            if col == row or row + col == 8:
                continue
            counter+=1
    assert len(briccks.all_bricks)==counter