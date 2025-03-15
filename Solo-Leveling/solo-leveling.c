#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int MAP_W = 10;
const int MAP_H = 6;
const int SPAWN_X = 2;
const int SPAWN_Y = 1;

typedef struct {
  int i;
  int j;
  char *name;
  char *desc;
  bool described;
  char *dialog;
  char *death;
  int hp;
  int attack;
} NPC;

NPC g_roaster = {.i = SPAWN_X,
                 .j = SPAWN_Y + 1,
                 .name = "Krag the Mocking Phantom",
                 .desc = "From the shadows, a voice snickers.",
                 .described = false,
                 .dialog = "NOOB! You gonna die, for sure!!",
                 .death = "You can't kill me that easily. Don't you have the "
                          "balls to face Igris?",
                 .hp = 0,
                 .attack = 0};
NPC g_chronicler = {
    .i = 7,
    .j = 0,
    .name = "Urahara the Chronicler",
    .desc = "Sitting on a weathered stone, a man cloaked in mystery hums an "
            "old tune. Urahara the Chronicler, his piercing eyes hidden "
            "beneath the brim of his hat, speaks in riddles.",
    .described = false,
    .dialog = "The gate has stood for centuries, yet within its walls lies a "
              "tale untold, Listen well, warrior. There is a secret hidden "
              "within my words--find it, and the path may yet open for you.",
    .death = "How was your death? Mindblowing..",
    .hp = 0,
    .attack = 0};
NPC g_shopkeeper = {
    .i = 0,
    .j = 5,
    .name = "Sakamoto the Wandering Merchant",
    .desc = "Nestled under the shadow of an ancient oak, a peculiar merchant "
            "sets up shop. Sakamoto the Wandering Merchant, dressed in "
            "tattered robes yet possessing an air of undeniable confidence, "
            "beckons the warrior closer.",
    .described = false,
    .dialog = "A blade, a shield, an enchantment perhaps? Strength alone will "
              "not defeat Igirs. You’ll need more than just courage to pierce "
              "his defenses.",
    .death = "You couldn't even survive one punch",
    .hp = 0,
    .attack = 0};
NPC g_boss = {
    .i = MAP_W - 1,
    .j = 3,
    .name = "Igris The Knight Commander",
    .desc = "Before this legendary gate stands Knight Commander Igirs, a "
            "towering figure clad in obsidian-black armor. His crimson cape "
            "flutters as he grips the hilt of his greatsword, eyes burning "
            "with the determination of a warrior who has never been defeated.",
    .described = false,
    .dialog = "You are not worthy, Prove yourself... or fall like the rest.",
    .death = "Oh, back already? Didn’t even last a minute! Maybe you should "
             "try using a real weapon this time?",
    .hp = 9999,
    .attack = 50};

struct State {
  char input[64];
  bool admin;
  bool running;
  int i;
  int j;
  char map[MAP_H][MAP_W + 1];
  int player_hp;
  char *feedback;
  bool view_map;
  int money;
  int sword;
  int armor;
  int spell[2];
  char *heap_buf;
} g_state;

bool is_near_npc(NPC npc) {
  if (abs(npc.i - g_state.i) <= 1 && abs(npc.j - g_state.j) <= 1) {
    return true;
  } else {
    return false;
  }
}

void initialize_game() {
  printf("\
____ ____ _    ____    _    ____ _  _ ____ _    _ _  _ ____ \n\
[__  |  | |    |  | __ |    |___ |  | |___ |    | |\\ | | __ \n\
___] |__| |___ |__|    |___ |___  \\/  |___ |___ | | \\| |__] \n\n");
  g_state.input[0] = '\0';
  g_state.admin = false;
  g_state.running = true;
  g_state.i = SPAWN_X;
  g_state.j = SPAWN_Y;
  for (int y = 0; y < MAP_H; y++) {
    for (int x = 0; x < MAP_W; x++) {
      g_state.map[y][x] = '.';
    }
    g_state.map[y][MAP_W] = '\0';
  }
  g_state.map[SPAWN_Y][SPAWN_X] = 'P';
  g_state.map[g_boss.j][g_boss.i] = g_boss.name[0];
  g_state.map[g_chronicler.j][g_chronicler.i] = g_chronicler.name[0];
  g_state.map[g_roaster.j][g_roaster.i] = g_roaster.name[0];
  g_state.map[g_shopkeeper.j][g_shopkeeper.i] = g_shopkeeper.name[0];
  g_state.player_hp = 100;
  g_state.view_map = true;
  g_state.money = 1000;
  g_state.sword = 0;
  g_state.armor = 0;
  g_state.spell[0] = 0;
  g_state.spell[1] = 0;
  g_state.heap_buf = malloc(128);
  g_state.feedback =
      "The wind howls across the barren battlefield, carrying whispers of "
      "warriors long forgotten. A massive stone gate looms in the distance, "
      "its iron reinforcements etched with runes of ancient power. Beyond it "
      "lies the Flag of Conquerors, a relic of untold power--one that only the "
      "strongest may claim.";
}

void display_output() {
  if (g_state.view_map) {
    for (int i = 0; i < MAP_H; i++) {
      printf("%s\n", g_state.map[i]);
    }
    printf("\n");
    g_state.view_map = false;
  }
  printf("%s\n\n", g_state.feedback);
  g_state.feedback = "";

  if (is_near_npc(g_shopkeeper)) {
    if (!g_shopkeeper.described) {
      printf("%s\n", g_shopkeeper.desc);
      g_shopkeeper.described = true;
    }
    printf("%s: %s\n\n", g_shopkeeper.name, g_shopkeeper.dialog);
  }
  if (is_near_npc(g_chronicler)) {
    if (!g_chronicler.described) {
      printf("%s\n", g_chronicler.desc);
      g_chronicler.described = true;
    }
    printf("%s: %s\n\n", g_chronicler.name, g_chronicler.dialog);
  }
  if (is_near_npc(g_roaster)) {
    if (!g_roaster.described) {
      printf("%s\n", g_roaster.desc);
      printf("%s: %s\n\n", g_roaster.name, g_roaster.dialog);
      g_roaster.described = true;
    }
  }
  if (is_near_npc(g_boss)) {
    if (!g_boss.described) {
      printf("%s\n", g_boss.desc);
      g_boss.described = true;
    }
    printf("%s: %s\n\n", g_boss.name, g_boss.dialog);
  }
  printf(">>> ");
  fflush(stdout);
}

void get_user_input() {
  gets(g_state.input);
  for (int i = 0; g_state.input[i]; i++) {
    g_state.input[i] = tolower((unsigned char)g_state.input[i]);
  }
}

bool v_north() {
  if (g_state.j > 0 && g_state.map[g_state.j - 1][g_state.i] == '.') {
    g_state.map[g_state.j][g_state.i] = '.';
    g_state.j--;
    g_state.map[g_state.j][g_state.i] = 'P';
    g_state.view_map = true;
    return true;
  } else {
    return false;
  }
}

bool v_south() {
  if (g_state.j < MAP_H - 1 && g_state.map[g_state.j + 1][g_state.i] == '.') {
    g_state.map[g_state.j][g_state.i] = '.';
    g_state.j++;
    g_state.map[g_state.j][g_state.i] = 'P';
    g_state.view_map = true;
    return true;
  } else {
    return false;
  }
}

bool v_east() {
  if (g_state.i < MAP_W - 1 && g_state.map[g_state.j][g_state.i + 1] == '.') {
    g_state.map[g_state.j][g_state.i] = '.';
    g_state.i++;
    g_state.map[g_state.j][g_state.i] = 'P';
    g_state.view_map = true;
    return true;
  } else {
    return false;
  }
}

bool v_west() {
  if (g_state.i > 0 && g_state.map[g_state.j][g_state.i - 1] == '.') {
    g_state.map[g_state.j][g_state.i] = '.';
    g_state.i--;
    g_state.map[g_state.j][g_state.i] = 'P';
    g_state.view_map = true;
    return true;
  } else {
    return false;
  }
}

bool v_northeast() { return v_north() || v_east(); }

bool v_southeast() { return v_south() || v_east(); }

bool v_northwest() { return v_north() || v_west(); }

bool v_southwest() { return v_south() || v_west(); }

void v_inventory() {
  sprintf(g_state.heap_buf,
          "Money: %d\nArmor: %d\nSword: %d\nSpell: value %d, turns %d\n",
          g_state.money, g_state.armor, g_state.sword, g_state.spell[0],
          g_state.spell[1]);
  g_state.feedback = g_state.heap_buf;
}

void v_quit() { g_state.running = false; }

void v_restart() { initialize_game(); }

void v_die(char *roast) {
  g_state.feedback = "YOU DIE";
  g_state.view_map = true;
  g_roaster.described = false;
  g_roaster.dialog = roast;
  g_state.map[g_state.j][g_state.i] = '.';
  g_state.j = SPAWN_Y;
  g_state.i = SPAWN_X;
  g_state.map[g_state.j][g_state.i] = 'P';
}

int v_win() {
  printf("As the final echoes of battle fade, a hush settles over the "
         "battlefield. The ancient stone gate, once an impassable barrier, "
         "groans open, revealing a lone pedestal bathed in ethereal light. "
         "Resting atop it is the Flag of Conquerors, its golden threads "
         "shimmering with an otherworldly glow.\n\n");
  printf(
      "With a steady breath, you step forward, each movement carrying the "
      "weight of your journey—the encounters, the battles, the choices made in "
      "the heat of desperation. The air crackles as your fingers brush against "
      "the fabric, and in that moment, visions flood your mind.\n\n");
  printf(
      "You see the ghosts of warriors long past, those who sought glory but "
      "fell before the might of Igris, The Knight Commander. Their spirits "
      "linger, not in anger, but in reverence. The Flag is more than a relic; "
      "it is a testament to those who dared to stand against fate itself.\n\n");
  printf("A deep voice rumbles from behind—Igris, now kneeling, his blade "
         "planted firmly in the ground. His once-intimidating presence is "
         "tempered by something new: respect.\n\n");
  printf("\"You have proven yourself worthy. Take the flag, but know this... "
         "true conquest is not of the battlefield, but of the soul. The path "
         "forward is yours to forge.\"\n\n");
  printf("As you clutch the Flag, the world seems to shift. The shadows of the "
         "past dissipate, and the battlefield begins to fade. The adventure "
         "has ended, yet something lingers—a sense of purpose, a hunger for "
         "what lies beyond the horizon.\n\n");
  printf("Your story does not end here. The winds of fate still carry whispers "
         "of greater trials to come.\n\n");
  printf("And so, the legend begins...\n\n");
  FILE *file = fopen("flag", "r");
  if (file == NULL) {
    perror("Error opening file");
  }
  char flag[64];
  if (fgets(flag, sizeof(flag), file) != NULL) {
    printf("Flag: %s\n", flag);
  } else {
    perror("Error reading flag.");
  }
  fclose(file);
  g_state.running = false;
}

int v_duel(NPC npc) {
  if (npc.hp <= 0) {
    v_die(npc.death);
  } else {
    int spell_turns_left = 0;
    int player_hp = g_state.player_hp;
    int npc_hp = npc.hp;
    bool spellback = false;
    char *feedback1 = "";
    char *feedback2 = "";
    char *buf1 = malloc(64);
    char *buf2 = malloc(64);
    while (player_hp > 0 && npc_hp > 0) {
      if (spellback)
        printf("You got affected by sideeffects");
      printf("%s\n%s\n\n", feedback1, feedback2);
      printf("Player HP: %d, Armor: %d, Boss HP: %d\n", player_hp,
             g_state.armor, npc_hp);
      if (spell_turns_left > 0) {
        printf("Spell active for %d turns (%s)\n\n", spell_turns_left,
               g_state.spell[0] > 0 ? "poison" : "heal");
      }
      printf("!!> ");
      fflush(stdout);
      get_user_input();
      int atk = 0;
      int dmg = 0;
      if (!strcmp(g_state.input, "attack")) {
        atk += g_state.sword;
        // sprintf(buf1, "You attack the boss for %d damage.\n", atk);
      } else if (!strcmp(g_state.input, "dodge") ||
                 !strcmp(g_state.input, "parry")) {
        dmg -= g_state.sword;
      } else if (!strcmp(g_state.input, "cast")) {
        if (spell_turns_left) {
          dmg += 20;
          spell_turns_left = 0;
          spellback = true;
        } else {
          spell_turns_left = g_state.spell[1];
        }
        sprintf(buf1, "%s\n", "You casted the spell");
      } else if (!strcmp(g_state.input, "heal")) {
        g_state.spell[0] = -abs(g_state.spell[0]);
        sprintf(buf1, "%s\n", "Switched to Heal mode");
      } else if (!strcmp(g_state.input, "poison")) {
        g_state.spell[0] = abs(g_state.spell[0]);
        sprintf(buf1, "%s\n", "Switched to Poison mode");
      } else if (!strcmp(g_state.input, "flee") ||
                 !strcmp(g_state.input, "run")) {
        sprintf(buf1, "YOU RAN AWAY\n");
        break;
      } else if (!strcmp(g_state.input, "arise")) {
        sprintf(buf1, "%s: %s\n", npc.name, "You are not worthy to say that");
        dmg += npc.attack;
      } else {
        printf("Invalid action.\n");
        continue;
      }
      if (spell_turns_left > 0) {
        if (g_state.spell[0] < 0) {
          dmg += g_state.spell[0];
        } else {
          atk += g_state.spell[0];
        }
        spell_turns_left--;
      }
      if (atk > 0)
        sprintf(buf1, "You attack Igris for %d damage.\n", atk);
      feedback1 = buf1;
      npc_hp -= atk;
      if (npc_hp > 0) {
        dmg += npc.attack;
        dmg = dmg > 0 ? dmg : 0;
        if (!dmg) {
          feedback2 = "You successfully dodge Igris' attack";
        } else {
          if (g_state.armor >= dmg) {
            g_state.armor -= dmg;
            feedback2 = "Igris attacks, but your armor absorbs all damage.\n";
          } else if (g_state.armor > 0) {
            int remaining_damage = dmg - g_state.armor;
            g_state.armor = 0;
            player_hp -= remaining_damage;
            player_hp = player_hp > 0 ? player_hp : 0;
            sprintf(buf2, "Igris attacks for %d damage, breaking your armor\n",
                    remaining_damage);
            feedback2 = buf2;
          } else {
            player_hp -= dmg;
            player_hp = player_hp > 0 ? player_hp : 0;
            sprintf(buf2, "Igris attack for %d damage.\n", dmg);
            feedback2 = buf2;
          }
        }
      }
      printf("\033[2J");
    }
    printf("%s\n", feedback1);
    free(buf1);
    free(buf2);
    if (player_hp <= 0) {
      v_die(npc.death);
    } else {
      printf("You defeated %s!\n", npc.name);
      v_win();
    }
  }
  return 0;
}

void parse_input() {
  char *verb = strtok(g_state.input, " ");
  char *obj = strtok(NULL, " ");
  if (verb == NULL)
    return;

  if (g_state.input[0] == '#' && g_state.admin) {
    if (!strcmp(verb, "#teleport") || !strcmp(verb, "#tp")) {
      char *x_str = obj;
      char *y_str = strtok(NULL, " ");
      if (x_str && y_str) {
        int x = atoi(x_str);
        int y = atoi(y_str);
        if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H &&
            g_state.map[y][x] == '.') {
          g_state.map[g_state.j][g_state.i] = '.';
          g_state.i = x;
          g_state.j = y;
          g_state.map[y][x] = 'P';
          g_state.view_map = true;
          sprintf(g_state.heap_buf, "[Teleport to (%d, %d)]", x, y);
          g_state.feedback = g_state.heap_buf;
        } else {
          g_state.feedback = "[Invalid teleport location]";
        }
      } else {
        g_state.feedback = "[Malformed Arguments]";
      }
    } else if (!strcmp(verb, "#set")) {
      char *what = obj;
      char *value_str = strtok(NULL, " ");
      if (what && value_str) {
        int value = atoi(value_str);
        if (!strcmp(what, "sword")) {
          g_state.sword = value;
          sprintf(g_state.heap_buf, "[Sword: %d]", g_state.sword);
          g_state.feedback = g_state.heap_buf;
        } else if (!strcmp(what, "armor")) {
          g_state.armor = value;
          sprintf(g_state.heap_buf, "[Armor: %d]", g_state.armor);
          g_state.feedback = g_state.heap_buf;
        } else if (!strcmp(what, "money")) {
          g_state.money = value;
          sprintf(g_state.heap_buf, "[Money: %d]", g_state.money);
          g_state.feedback = g_state.heap_buf;
        } else if (!strcmp(what, "myhp")) {
          g_state.player_hp = value;
          sprintf(g_state.heap_buf, "[HP: %d]", g_state.player_hp);
          g_state.feedback = g_state.heap_buf;
        } else if (!strcmp(what, "hishp")) {
          g_boss.hp = value;
          sprintf(g_state.heap_buf, "[Boss HP: %d]", g_boss.hp);
          g_state.feedback = g_state.heap_buf;
        } else if (!strcmp(what, "spell")) {
          g_state.spell[0] = value;
          sprintf(g_state.heap_buf, "[%s: %d]",
                  g_state.spell[0] > 0 ? "Poison" : "Heal",
                  abs(g_state.spell[0]));
          g_state.feedback = g_state.heap_buf;
        } else if (!strcmp(what, "spelldur")) {
          g_state.spell[1] = value;
          sprintf(g_state.heap_buf, "[Spell Duration: %d]", g_state.spell[1]);
          g_state.feedback = g_state.heap_buf;
        } else {
          g_state.feedback = "[Invalid argument]";
        }
      } else {
        g_state.feedback = "[Malformed Arguments]";
      }
    }
  } else if (!strcmp(verb, "n") || !strcmp(verb, "north")) {
    if (!v_north()) {
      sprintf(g_state.heap_buf, "Can't move %s\n", verb);
      g_state.feedback = g_state.heap_buf;
    }
  } else if (!strcmp(verb, "s") || !strcmp(verb, "south")) {
    if (!v_south()) {
      sprintf(g_state.heap_buf, "Can't move %s\n", verb);
      g_state.feedback = g_state.heap_buf;
    }
  } else if (!strcmp(verb, "e") || !strcmp(verb, "east")) {
    if (!v_east()) {
      sprintf(g_state.heap_buf, "Can't move %s\n", verb);
      g_state.feedback = g_state.heap_buf;
    }
  } else if (!strcmp(verb, "w") || !strcmp(verb, "west")) {
    if (!v_west()) {
      sprintf(g_state.heap_buf, "Can't move %s\n", verb);
      g_state.feedback = g_state.heap_buf;
    }
  } else if (!strcmp(verb, "arise")) {
    g_state.feedback = "...No one cares";
  } else if (!strcmp(verb, "i") || !strcmp(verb, "inv") ||
             !strcmp(verb, "inventory")) {
    v_inventory();
  } else if (!strcmp(verb, "quit") || !strcmp(verb, "q")) {
    v_quit();
  } else if (!strcmp(verb, "restart")) {
    v_restart();
  } else if (!strcmp(verb, "duel")) {
    if (is_near_npc(g_roaster)) {
      v_duel(g_roaster);
    } else if (is_near_npc(g_shopkeeper)) {
      v_duel(g_shopkeeper);
    } else if (is_near_npc(g_chronicler)) {
      v_duel(g_chronicler);
    } else if (is_near_npc(g_boss)) {
      v_duel(g_boss);
    } else {
      g_state.feedback = "No one to fight with..";
    }
  } else if (!strcmp(verb, "die") || !strcmp(verb, "suicide")) {
    v_die("Coward! You took your own life.");
  } else if (!strcmp(verb, "look") || !strcmp(verb, "map")) {
    g_state.view_map = true;
  } else if (!strcmp(verb, "buy") || !strcmp(verb, "enchant")) {
    if (is_near_npc(g_shopkeeper)) {
      if (!strcmp(verb, "buy")) {
        if (obj) {
          if (!strcmp(obj, "sword")) {
            if (g_state.money >= 100) {
              g_state.sword += 10;
              g_state.money -= 100;
              sprintf(g_state.heap_buf, "Bought sword, sword now %d",
                      g_state.sword);
              g_state.feedback = g_state.heap_buf;
            } else {
              g_state.feedback = "Not enough money";
            }
          } else if (!strcmp(obj, "armor")) {
            if (g_state.money >= 100) {
              g_state.armor += 50;
              g_state.money -= 100;
              sprintf(g_state.heap_buf, "Bought armor, armor now %d",
                      g_state.armor);
              g_state.feedback = g_state.heap_buf;
            } else {
              g_state.feedback = "Not enough money";
            }
          } else if (!strcmp(obj, "spell")) {
            if (g_state.money >= 100) {
              g_state.spell[0] += g_state.spell[0] > 0 ? 10 : -10;
              g_state.money -= 100;
              g_state.feedback = "Bought spell";
            } else {
              g_state.feedback = "Not enough money";
            }
          } else {
            g_state.feedback = "Unknown item";
          }
        } else {
          g_state.feedback = "Buy what?";
        }
      } else {
        if (g_state.money >= 50) {
          g_state.spell[1] += 1;
          g_state.money -= 50;
          g_state.feedback = "Enchanted spell";
        } else {
          g_state.feedback = "Not enough money";
        }
      }
    } else {
      g_state.feedback = "No shop here";
    }
  } else if (!strcmp(verb, "fuck") || !strcmp(verb, "curse") ||
             !strcmp(verb, "shit") || !strcmp(verb, "damn")) {
    g_state.feedback = "Thats very rude";
  } else if (!strcmp(verb, "rape")) {
    if (is_near_npc(g_roaster))
      v_die("Thats what you get for trying to rape me");
    else if (is_near_npc(g_shopkeeper) || is_near_npc(g_chronicler))
      v_die("Thats what you get for trying to rape");
    else if (is_near_npc(g_boss))
      v_die(
          "You literally tried to rape someone who is far stronger than you.");
    else
      g_state.feedback = "No one to rape";
  } else if (!strcmp(verb, "hello") || !strcmp(verb, "hi")) {
    if (is_near_npc(g_roaster)) {
      sprintf(g_state.heap_buf, "%s: ...\n", g_roaster.name);
      g_state.feedback = g_state.heap_buf;
    } else if (is_near_npc(g_chronicler)) {
      sprintf(g_state.heap_buf, "%s greets you back.\n", g_chronicler.name);
      g_state.feedback = g_state.heap_buf;
    } else if (is_near_npc(g_shopkeeper)) {
      sprintf(g_state.heap_buf, "%s greets you back.\n", g_shopkeeper.name);
      g_state.feedback = g_state.heap_buf;
    } else if (is_near_npc(g_boss)) {
      sprintf(g_state.heap_buf, "%s: Do you want to get a taste of my blade?\n",
              g_boss.name);
      g_state.feedback = g_state.heap_buf;
    }
  } else if (!strcmp(verb, "describe")) {
    if (is_near_npc(g_roaster))
      g_roaster.described = false;
    if (is_near_npc(g_chronicler))
      g_chronicler.described = false;
    if (is_near_npc(g_shopkeeper))
      g_shopkeeper.described = false;
    if (is_near_npc(g_boss))
      g_boss.described = false;
  } else {
    g_state.feedback = "Unknown command";
  }
}

void process_input() {
  get_user_input();
  parse_input();
  // printf("\033[2J");
}

int main() {
  printf("\033[2J");
  initialize_game();
  while (g_state.running) {
    display_output();
    process_input();
  }
  free(g_state.heap_buf);
  return 0;
}