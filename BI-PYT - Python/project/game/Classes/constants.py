import pygame

field_width = 652
field_start = 50
screen_width = 1000
screen_height = 700
screen2_width = 652
screen2_height = 650
bg_color = (255, 255, 0)
field_color = (0, 0, 0)

brick_start = 87
brick_cols = 9
brick_row = 9

ball_half = 11
laser_half_height = 32
paddle_half_heigh = 12
paddle_half_width = 52

paddle_x_pos = field_width / 2 + field_start
paddle_y_pos = 650
ball_x_pos = field_width / 2 + field_start
ball_y_pos = paddle_y_pos - paddle_half_heigh - ball_half

paddle_left_border = field_start + paddle_half_width
paddle_right_border = field_width + field_start - paddle_half_width

ball_left_border = ball_half + field_start
ball_right_border = field_width + field_start - ball_half

pygame.font.init()
banner_font = pygame.font.Font(None, 100)
score_font = pygame.font.Font(None, 40)
small_font = pygame.font.Font(None, 20)

text_start = banner_font.render("PRESS ENTER TO START", True, pygame.Color("white"))
text_again = score_font.render("PRESS ENTER TO PLAY AGAIN", True, pygame.Color("white"))
text_next_level = score_font.render("NEXT LEVEL: PRESS ENTER", True, pygame.Color("white"))
text_fail = banner_font.render("YOU LOSE!", True, pygame.Color("white"))
