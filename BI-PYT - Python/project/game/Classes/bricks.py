import pygame
from random import randint
import game.Classes.constants as constants

bricks = ['Sprites/element_green_rectangle.png', 'Sprites/element_purple_rectangle.png',
          'Sprites/element_red_rectangle.png', 'Sprites/element_yellow_rectangle.png']


class Brick(pygame.sprite.Sprite):
    """
    Represents single brick

    Attributes
    ____________
    image: image
        image of the brick
    rect: rect
        borders of image
    brick_x: int
        x position of brick
    brick_y: int
        y position of brick
    rect.center: (int,int)
        center coordinates of rect
    """

    def __init__(self, row, col):
        """
        Parameters
        ____________
        row: int
            number of rows
        col: int
            number of columns
        """
        super().__init__()
        self.brick_x = constants.brick_start + 32 + (64 * col)
        self.brick_y = constants.brick_start + 16 + (32 * row)
        self.image = pygame.image.load(bricks[randint(0, 3)])
        self.rect = self.image.get_rect()
        self.rect.center = (self.brick_x, self.brick_y)


class Bricks:
    """
    Represents all bricks, sets their position a makes group from them

    Attributes
    ___________
    all_sprites: sprite group
        group of all  sprites
    all_bricks: sprite group
        group of brick sprites

    Methods
    ________
    check_collision
        checks if ball and brick collides
        returns: True -if they collide

    """

    def __init__(self, all_sprites):
        """
        Parameters
        ___________
        all_sprites: sprite
            group of all sprites
        """
        self.all_sprites = all_sprites
        self.all_bricks = pygame.sprite.Group()
        for row in range(constants.brick_row):
            for col in range(constants.brick_cols):
                if col == row or row + col == 8:
                    continue
                brick = Brick(row, col)
                self.all_bricks.add(brick)
                self.all_sprites.add(brick)

    def check_collision(self, ball):
        """
        Parameters
        ___________
        ball: Ball class instance
        """
        for brick in self.all_bricks:
            if brick.rect.colliderect(ball):
                if abs(brick.rect.left - ball.rect.right) < constants.paddle_half_heigh or abs(
                        brick.rect.right - ball.rect.left) < 12:
                    ball.bounce_side()
                    brick.kill()
                    return True
                if abs(brick.rect.top - ball.rect.bottom) < constants.paddle_half_heigh or abs(
                        brick.rect.bottom - ball.rect.top) < 12:
                    ball.bounce_up()
                    brick.kill()
                    return True
