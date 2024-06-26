import pygame
from game.Classes.paddle import Paddle
from game.Classes.ball import Ball
from game.Classes.bricks import Bricks
from random import randint
from game.Classes.laser import Laser
import game.Classes.constants as constants


def paused():
    """
    Pauses and unpauses game if you press space

    returns: if you close the game
    """
    pause = True
    while pause:
        pressed_keys = pygame.key.get_pressed()
        for event in pygame.event.get():
            if pressed_keys[pygame.K_SPACE]:
                pause = False
            elif event.type == pygame.QUIT:
                return


def ball_brick_collision(score, lasers, shootable, collision=None):
    """
    Parameters
    ________
    score: int
        your current score
    lasers: int
        count of lasers available
    shootable: bool
        if you are able to shoot a laser
    collision: sound of collision

    Returns
    _________
    score: int
        your current score
    lasers: int
        count of lasers available
    shootable: bool
        if you are able to shoot a laser
    """

    score += 100
    if collision is not None:
        collision.play()
    if randint(1, 10) == 10:
        shootable = True
        lasers += 5
        return score, lasers, shootable
    else:
        return score, lasers, shootable


def shoot_laser(screen, my_laser, my_bricks, shootable, score, fire):
    """
    Parameters
    ________
    score: int
        your current score
    shootable: bool
        if you are able to shoot a laser
    screen: screen
    my_laser: Laser class instance
    my_bricks: Bricks class instance

    Returns
    _________
    score: int
        your current score
    shootable: bool
        if you are able to shoot a laser
    """

    screen.blit(my_laser.image, (my_laser.laser_x, my_laser.laser_y))
    shootable = False
    if my_laser.check_collision(my_bricks.all_bricks):
        score += 100
        fire = False
    return shootable, score, fire


def next_level(screen, pressed_keys, all_sprites, my_paddle, my_ball, lasers, fire, shootable, my_bricks):
    """
    Parameters
    ________
    screen: screen
    pressed_keys: all pressed keys in the moment
    all_sprites: sprite group
        all sprites
    my_paddle: Paddle class instance
    my_ball: Ball class instance
    lasers: int
        count of lasers available
    fire: bool
        tell if the laser was shot
    shootable: bool
        if you are able to shoot a laser
    my_bricks: Bricks class instance

    Returns
    _________
    able_to_die: bool
        if you can lose
    lasers: int
        count of lasers available
    fire: bool
        tell if the laser was shot
    shootable: bool
        if you are able to shoot a laser
    my_bricks: Bricks class instance
    """

    able_to_die = False
    screen.blit(constants.text_next_level, (
        constants.screen_width / 2 - constants.text_next_level.get_width() / 2 - 100, constants.screen_height / 2))
    if pressed_keys[pygame.K_RETURN]:
        all_sprites.empty()
        my_paddle.reset()
        my_ball.reset()
        all_sprites.add(my_paddle, my_ball)
        my_bricks = Bricks(all_sprites)
        lasers = 0
        fire = False
        shootable = False
        able_to_die = True
        return able_to_die, lasers, fire, shootable, my_bricks
    else:
        return able_to_die, lasers, fire, shootable, my_bricks


def fail(screen, pressed_keys, all_sprites, my_paddle, my_ball, score, lasers, fire, shootable, my_bricks):
    """
    Parameters
    ________
    screen: screen
    pressed_keys: all pressed keys in the moment
    all_sprites: sprite group
        all sprites
    my_paddle: Paddle class instance
    my_ball: Ball class instance
    score: int
        your current score
    lasers: int
        count of lasers available
    fire: bool
        tell if the laser was shot
    shootable: bool
        if you are able to shoot a laser
    my_bricks: Bricks class instance

    Returns
    _________
    score: int
        your current score
    lasers: int
        count of lasers available
    fire: bool
        tell if the laser was shot
    shootable: bool
        if you are able to shoot a laser
    my_bricks: Bricks class instance
    """

    pygame.mixer.music.pause()
    screen.blit(constants.text_fail,
                (constants.screen_width / 2 - constants.text_fail.get_width() / 2 - 100, constants.screen_height / 2))
    screen.blit(constants.text_again, (
        constants.screen_width / 2 - constants.text_again.get_width() / 2 - 100, constants.screen_height / 2 + 100))
    if pressed_keys[pygame.K_RETURN]:
        all_sprites.empty()
        my_paddle.reset()
        my_ball.reset()
        score = 0
        lasers = 0
        fire = False
        shootable = False
        all_sprites.add(my_paddle, my_ball)
        my_bricks = Bricks(all_sprites)
        pygame.mixer.music.unpause()
        return score, lasers, fire, shootable, my_bricks
    else:
        return score, lasers, fire, shootable, my_bricks


def main():
    pygame.init()

    screen = pygame.display.set_mode((constants.screen_width, constants.screen_height))
    screen2 = pygame.Surface([constants.screen2_width, constants.screen2_height])

    pygame.display.set_caption("ARKANOID")
    icon = pygame.image.load("Sprites/arkanoid (1).png")
    pygame.display.set_icon(icon)

    my_paddle = Paddle()
    my_ball = Ball()
    all_sprites = pygame.sprite.Group()
    all_sprites.add(my_paddle, my_ball)
    my_bricks = Bricks(all_sprites)
    my_laser = Laser(my_paddle.paddle_x, my_paddle.paddle_y)

    pygame.mixer.pre_init(frequency=44100, size=-16, channels=2, buffer=512)
    pygame.mixer.music.load('Sounds/SLOWEST-TEMPO2019-12-11_-_Retro_Platforming_-_David_Fesliyan.mp3')
    pygame.mixer.music.play(-1)
    collision = pygame.mixer.Sound('Sounds/270326__littlerobotsoundfactory__hit-01.wav')

    clock = pygame.time.Clock()
    score = 0
    able_to_die = True
    fire = False
    shootable = False
    lasers = 0

    # intro screen
    while True:
        pressed_keys = pygame.key.get_pressed()
        if pressed_keys[pygame.K_RETURN]:
            break
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                return
        screen.blit(constants.text_start,
                    (constants.screen_width / 2 - constants.text_start.get_width() / 2, constants.screen_height / 2))
        clock.tick(60)
        pygame.display.update()

    # game
    while True:

        pressed_keys = pygame.key.get_pressed()

        if pressed_keys[pygame.K_a]:
            my_paddle.left()
        if pressed_keys[pygame.K_d]:
            my_paddle.right()
        for event in pygame.event.get():
            if pressed_keys[pygame.K_SPACE]:
                paused()
            if event.type == pygame.QUIT:
                return
            if shootable and event.type == pygame.KEYDOWN:
                if event.key == pygame.K_w:
                    my_laser.laser_x = my_paddle.paddle_x - 16
                    fire = True
                    lasers -= 1
                    if lasers == 0:
                        shootable = False

        if my_laser.update(fire) and fire:
            shootable, score, fire = shoot_laser(screen, my_laser, my_bricks, shootable, score, fire)
        else:
            fire = False
            if lasers > 0:
                shootable = True

        if my_ball.check_collision(my_paddle):
            collision.play()

        if my_bricks.check_collision(my_ball):
            score, lasers, shootable = ball_brick_collision(score, lasers, shootable, collision)

        if not my_bricks.all_bricks:
            able_to_die, lasers, fire, shootable, my_bricks = next_level(screen, pressed_keys, all_sprites, my_paddle,
                                                                         my_ball, lasers, fire, shootable, my_bricks)

        all_sprites.update()
        pygame.display.update()
        clock.tick(60)

        if my_ball.gonna_lose() and able_to_die:
            score, lasers, fire, shootable, my_bricks = fail(screen, pressed_keys, all_sprites, my_paddle, my_ball,
                                                             score, lasers, fire, shootable, my_bricks)

        else:
            screen.fill(constants.bg_color)
            screen2.fill(constants.field_color)
            score_text = constants.score_font.render(f"SCORE: {score}", True, pygame.Color("black"))
            screen.blit(score_text, (750, 100))
            laser_text = constants.small_font.render(f"PRESS (w) to use {lasers} lasers", True, pygame.Color("black"))
            screen.blit(laser_text, (750, 200))
            pygame.Surface.blit(screen, screen2, (50, 50))
            all_sprites.draw(screen)


if __name__ == '__main__':
    main()
