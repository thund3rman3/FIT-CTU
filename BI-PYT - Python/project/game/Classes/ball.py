import pygame
from random import randint
import game.Classes.constants as constants


class Ball(pygame.sprite.Sprite):
    """
    Represents ball, which is bouncing from suface

    Attributes
    __________
    image: image
        image of the ball
    rect: rect
        borders of image
    ball_x: int
        x position of ball
    ball_y: int
        y position of ball
    rect.center: (int,int)
        center coordinates of rect
    movement:   (int,int)
        coordinates of ball

    Methods
    ________
    update
        checks if the ball is not out of borders of field
        moves the ball, increases x,y by movement coordinates
    check_collision
        checks if paddle and ball are colliding
        returns: True -if they collide
    gonna_lose
        returns:True -if ball passed bottom border and the game is going to end
    reset
        resets the position of the ball to the starting position
    bounce_up
        inverts ball y coordinate
    bounce_side
        inverts ball x coordinate
    """

    def __init__(self):
        super().__init__()
        self.image = pygame.image.load('Sprites/ballBlue.png')
        self.rect = self.image.get_rect()
        self.ball_x = constants.ball_x_pos
        self.ball_y = constants.ball_y_pos
        self.rect.center = (self.ball_x, self.ball_y)
        self.movement = [randint(3, 5), randint(3, 5)]

    def update(self):

        if self.ball_x < constants.ball_left_border or self.ball_x > constants.ball_right_border:
            self.movement[0] = -self.movement[0]
        if self.ball_y < constants.ball_half + constants.field_start:
            self.movement[1] = -self.movement[1]

        self.ball_x += self.movement[0]
        self.ball_y += self.movement[1]
        self.rect.center = (self.ball_x, self.ball_y)

    def check_collision(self, paddle):
        """
        Parameters
        ____________
        paddle: Paddle class instance
        """
        max_ball_speed = 6

        if self.rect.colliderect(paddle.rect):
            if abs(self.rect.bottom - paddle.rect.top) < constants.paddle_half_heigh and self.movement[1] > 0:
                self.movement[1] = self.movement[1]*(-1)
                self.movement[0] += paddle.direction
                return True

    def gonna_lose(self):

        if self.ball_y > constants.screen_height:
            return True

    def reset(self):

        self.movement = [6, 6]
        self.ball_x = constants.ball_x_pos
        self.ball_y = constants.ball_y_pos
        self.rect.center = (self.ball_x, self.ball_y)

    def bounce_up(self):
        self.movement[1] = self.movement[1] * (-1)

    def bounce_side(self):
        self.movement[0] = self.movement[0] * (-1)
