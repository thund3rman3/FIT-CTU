import pygame
import game.Classes.constants as constants


class Paddle(pygame.sprite.Sprite):
    """
    A class used to reperesent player/paddle

    Attributes
    __________
    image: image
        image of the paddle
    rect: rect
        borders of image
    paddle_x: int
        x position of paddle
    paddle_y: int
        y position of paddle
    direction: int
        in ball class
        changes x coordinate of ball when ball hits the paddle
    rect.center: (int,int)
        center coordinates of rect

    Methods
    ___________
    update
        checks if the paddle is not out of the plazing field
    left
        moves paddle left
    right
        moves paddle right
    reset
        resets the position of the paddle to starting position
    """

    def __init__(self):

        super().__init__()
        self.image = pygame.image.load("Sprites/paddleBlu.png")
        self.rect = self.image.get_rect()
        self.paddle_x = constants.paddle_x_pos
        self.paddle_y = constants.paddle_y_pos
        self.direction = 0
        self.rect.center = (self.paddle_x, self.paddle_y)

    def update(self):

        if self.paddle_x > constants.paddle_right_border:
            self.paddle_x = constants.paddle_right_border
        if self.paddle_x < constants.paddle_left_border:
            self.paddle_x = constants.paddle_left_border
        self.rect.center = (self.paddle_x, self.paddle_y)

    def left(self):
        self.paddle_x -= 10
        self.direction = -1

    def right(self):
        self.paddle_x += 10
        self.direction = 1

    def reset(self):
        self.paddle_x = constants.paddle_x_pos
        self.paddle_y = constants.paddle_y_pos
        self.rect.center = (self.paddle_x, self.paddle_y)
