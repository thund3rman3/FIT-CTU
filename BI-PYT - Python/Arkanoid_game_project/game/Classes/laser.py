import pygame
import game.Classes.constants as constants


class Laser(pygame.sprite.Sprite):
    """
    Represents laser shot

    Attributes
    ____________
    image: image
        image of the laser
    rect: rect
        borders of image
    laser_x: int
        x position of laser
    laser_y: int
        y position of laser
    rect.center: (int,int)
        center coordinates of rect

    Methods
    ________
    check_collision
        checks if ball and brick collides
        returns: True -if they collide
    update
        if laser hits top of the field and laser was shot, then laser position is set to origin
        elif laser was shot, then laser moves upwards
        returns:False -if laser hits top of the field and laser was shot
        returns:True  -elif laser was shot

    """

    def __init__(self, paddle_x, paddle_y):
        """
        Parameters
        ____________
        paddle_x: int
            x coordinate of paddle
        paddle_y: int
            y coordinate of paddle
        """

        super().__init__()
        self.laser_x = paddle_x
        self.laser_y = paddle_y + constants.paddle_half_heigh + constants.laser_half_height
        self.image = pygame.image.load('Sprites/laser.png')
        self.rect = self.image.get_rect()
        self.rect.center = (self.laser_x, self.laser_y)

    def check_collision(self, all_bricks):
        """
        Parameters
        ____________
        all_bricks: sprite group
        """

        for brick in all_bricks:
            if self.rect.colliderect(brick.rect):
                brick.kill()
                self.laser_y = constants.screen2_height - constants.paddle_half_heigh - 64
                return True

    def update(self, fire):
        """
        Parameters
        ____________
        fire: bool
            tell if the laser was shot
        """

        if self.laser_y <= constants.field_start and fire:
            self.laser_y = constants.screen2_height - constants.paddle_half_heigh - 64
            self.rect.center = (self.laser_x, self.laser_y)
            return False

        elif fire:
            self.laser_y -= 10
            self.rect.center = (self.laser_x, self.laser_y)
            return True
