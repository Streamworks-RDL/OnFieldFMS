import pygame, serial, pygame_menu
pygame.init()

ser = serial.Serial('/dev/ttyACM0', 921600, timeout=1)

size = 32

screen = pygame.display.set_mode((size*20,size*20))
square = pygame.Surface((size,size))
small_square = pygame.Surface((int(size/2),int(size/2)))

DRED_Y_MAX = 100
DRED_CB_MAX = 10
DRED_CR_MAX = 255

DRED_Y_MIN = -255
DRED_CB_MIN = -255
DRED_CR_MIN = 50

DBLUE_Y_MAX = 100
DBLUE_CB_MAX = 255
DBLUE_CR_MAX = 100

DBLUE_Y_MIN = -255
DBLUE_CB_MIN = 10
DBLUE_CR_MIN = -255

def set_DRED_Y(value):
    global DRED_Y_MIN, DRED_Y_MAX
    DRED_Y_MIN = int(value[0])
    DRED_Y_MAX = int(value[1])
def set_DRED_CB(value):
    global DRED_CB_MIN, DRED_CB_MAX
    DRED_CB_MIN = int(value[0])
    DRED_CB_MAX = int(value[1])
def set_DRED_CR(value):
    global DRED_CR_MIN, DRED_CR_MAX
    DRED_CR_MIN = int(value[0])
    DRED_CR_MAX = int(value[1])

def set_DBLUE_Y(value):
    global DBLUE_Y_MIN, DBLUE_Y_MAX
    DBLUE_Y_MIN = int(value[0])
    DBLUE_Y_MAX = int(value[1])
def set_DBLUE_CB(value):
    global DBLUE_CB_MIN, DBLUE_CB_MAX
    DBLUE_CB_MIN = int(value[0])
    DBLUE_CB_MAX = int(value[1])
def set_DBLUE_CR(value):
    global DBLUE_CR_MIN, DBLUE_CR_MAX
    DBLUE_CR_MIN = int(value[0])
    DBLUE_CR_MAX = int(value[1])

def rgbify(y, cb, cr):
    r = int(y) + 1.402*int(cr) + 128
    g = int(y) - 0.344*int(cb) - 0.714*int(cr) + 128
    b = int(y) + 1.772*int(cb) + 128
    r = max(0, min(255, r))
    g = max(0, min(255, g))
    b = max(0, min(255, b))
    return (r, g, b)

def match(y, cb, cr, color):
    if color == 'r':
        if (DRED_Y_MIN < int(y) < DRED_Y_MAX and
            DRED_CB_MIN < int(cb) < DRED_CB_MAX and
            DRED_CR_MIN < int(cr) < DRED_CR_MAX):
            return True
    if color == 'b':
        if (DBLUE_Y_MIN < int(y) < DBLUE_Y_MAX and
            DBLUE_CB_MIN < int(cb) < DBLUE_CB_MAX and
            DBLUE_CR_MIN < int(cr) < DBLUE_CR_MAX):
            return True
    return False


def main():

    mytheme = pygame_menu.themes.THEME_DARK.copy()
    mytheme.title_bar_style = pygame_menu.widgets.MENUBAR_STYLE_NONE
    menu = pygame_menu.Menu('', size*20, size*5, rows=6, columns=2, position=(0, size*15, False), theme=mytheme)

    menu.add.range_slider('DRED_Y', (DRED_Y_MIN, DRED_Y_MAX), (-255, 255), 1, value_format=lambda x: str(int(x)), width=size*8, onchange=set_DRED_Y, border_width=0, range_text_value_enabled=False)
    menu.add.range_slider('DRED_CB', (DRED_CB_MIN, DRED_CB_MAX), (-255, 255), 1, value_format=lambda x: str(int(x)), width=size*8, onchange=set_DRED_CB, border_width=0, range_text_value_enabled=False)
    menu.add.range_slider('DRED_CR', (DRED_CR_MIN, DRED_CR_MAX), (-255, 255), 1, value_format=lambda x: str(int(x)), width=size*8, onchange=set_DRED_CR, border_width=0, range_text_value_enabled=False)

    menu.add.range_slider('DBLUE_Y', (DBLUE_Y_MIN, DBLUE_Y_MAX), (-255, 255), 1, value_format=lambda x: str(int(x)), width=size*8, onchange=set_DBLUE_Y, border_width=0, range_text_value_enabled=False)
    menu.add.range_slider('DBLUE_CB', (DBLUE_CB_MIN, DBLUE_CB_MAX), (-255, 255), 1, value_format=lambda x: str(int(x)), width=size*8, onchange=set_DBLUE_CB, border_width=0, range_text_value_enabled=False)
    menu.add.range_slider('DBLUE_CR', (DBLUE_CR_MIN, DBLUE_CR_MAX), (-255, 255), 1, value_format=lambda x: str(int(x)), width=size*8, onchange=set_DBLUE_CR, border_width=0, range_text_value_enabled=False)

    while True:
        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            if event.type == pygame.KEYDOWN and event.key == pygame.K_y:
                eval(input())
        f = ser.readline().decode().strip()
        jpg = f.strip(",").split(",")
        for i, mcu in enumerate(jpg):
            try:
                y0, y1, cb0, cb1, cr0, cr1 = mcu.strip("|").split("|")
            except Exception:
                continue
            #print(rgbify(y0,cb0,cr0))
            #print(rgbify(y1,cb1,cr1))
            #print()
            square.fill(rgbify(y0,cb0,cr0))
            screen.blit(square, (i%10*2*size, int(i/10)*size))
            square.fill(rgbify(y1,cb1,cr1))
            screen.blit(square, (i%10*2*size+size, int(i/10)*size))

            if match(y0, cb0, cr0, 'b'):
                small_square.fill((0,255,0))
                screen.blit(small_square, (i%10*2*size + int(size/4), int(i/10)*size + int(size/4)))
            if match(y1, cb1, cr1, 'b'):
                small_square.fill((0,255,0))
                screen.blit(small_square, (i%10*2*size+size + int(size/4), int(i/10)*size + int(size/4)))

            if match(y0, cb0, cr0, 'r'):
                small_square.fill((200,0,255))
                screen.blit(small_square, (i%10*2*size + int(size/4), int(i/10)*size + int(size/4)))
            if match(y1, cb1, cr1, 'r'):
                small_square.fill((200,0,255))
                screen.blit(small_square, (i%10*2*size+size + int(size/4), int(i/10)*size + int(size/4)))
            #print(components)

        if menu.is_enabled():
            menu.update(events)
            menu.draw(screen)

        pygame.display.update()

if __name__ == "__main__":
    main()
