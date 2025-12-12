//
// Created by user on 27/10/2025.
//

#ifndef CLASSES_H
#define CLASSES_H
#include <cmath>
#include <SFML/Graphics.hpp>
#include <utility>

#endif //CLASSES_H
#include <iostream>

class Items {
public:
    bool have=false;
    bool effecton=false;
    int price;
    std::string name, descripction;

};

class Button {

private:
    sf::Vector2f position_;
    sf::Vector2f size_;

public:
    Button(float x = 0.f, float y = 0.f, float w = 0.f, float h = 0.f)
        : position_(x, y), size_(w, h) {}

    void setPosition(float x, float y) { position_.x = x; position_.y = y; }
    void setSize(float w, float h) { size_.x = w; size_.y = h; }

    const sf::Vector2f& position() const { return position_; }
    const sf::Vector2f& size() const { return size_; }

    bool contains(const sf::Vector2i &mouse) const {
        return mouse.x >= position_.x &&
               mouse.y >= position_.y &&
               mouse.x <= position_.x + size_.x &&
               mouse.y <= position_.y + size_.y;
    }
};

class Entity {
protected:
    float hp_;
    float maxhp_;
    float atk_;
    int gold_;
    int exp_;
    bool isDead_;

public:
    Entity(float hp = 100.f, float atk = 10.f, int gold = 0, int exp = 0)
        : hp_(hp), maxhp_(hp), atk_(atk), gold_(gold), exp_(exp), isDead_(false) {}

    float getHp() const { return hp_; }
    float getMaxHp() const { return maxhp_; }
    float getAtk() const { return atk_; }
    int getGold() const { return gold_; }
    int getExp() const { return exp_; }
    bool getIsDead() const { return isDead_; }

    void setHp(float h) { hp_ = h; }
    void setMaxHp(float mh) { maxhp_ = mh; }
    void setAtk(float a) { atk_ = a; }
    void setGold(int g) { gold_ = g; }
    void setExp(int e) { exp_ = e; }
    void setIsDead(bool d) { isDead_ = d; }

    virtual int DoDamage(float multiplier) {
        return static_cast<int>(std::floor(atk_ * multiplier));
    }

    virtual int TakeDamage(float dmg) {
        float newHp = hp_ - dmg;
        setHp(newHp);
        return static_cast<int>(std::ceil(hp_));
    }
};

class Leveling {
public:
    virtual void addExp(int amount) = 0;
    virtual bool tryLvlUp() = 0;
    virtual ~Leveling() = default;
};

class Player : public Entity, public Leveling {
private:
    int lvl;
    int lvlreq;
    int sp_;

public:
    Player()
        : Entity(100.f, 10.f, 0, 0), lvl(1), lvlreq(20), sp_(0) {}

    void addExp(int amount) override {
        setExp(getExp() + amount);
    }

    bool tryLvlUp() override {
        if (getExp() >= lvlreq) {
            lvl++;
            setExp(getExp() - lvlreq);
            lvlreq = lvlreq + (pow(lvl, 1.1)); // example progression
            setMaxHp(getMaxHp() + 30.f);
            setHp(getMaxHp());
            setAtk(getAtk() + 4.f);
            return true;
        }
        return false;
    }

    int getLevel() const { return lvl; }
    int getLevelRequirement() const { return lvlreq; }

    // Stamina / special points used in combat
    int getSP() const { return sp_; }
    void addSP(int v) { sp_ =  sp_ + v;}
    bool consumeSP(int cost) {
        if (sp_ >= cost) { sp_ -= cost; return true; }
        return false;
    }
};

class Enemy : public Entity {
private:
    int Enemylvl;

public:
    Enemy(float hp, float atk, int exp, int gold, int lvl = 1)
        : Entity(hp, atk, gold, exp), Enemylvl(lvl) {}

    int getLevel() const { return Enemylvl; }

    // Scale stats by multiplier (utility)
    void scaleByLevel(int level) {
        float factor = std::pow(1.3f, static_cast<float>(level));
        setAtk(getAtk() * factor);
        setHp(getHp() * factor);
        setMaxHp(getMaxHp() * factor);
        setExp(getExp() * level);
        setGold(getGold() * std::max(1, level/2));
    }
};

class GraphicsStoreUI {
public:
    virtual void update(const Player &player, const Items &i1, const Items &i2) = 0;
    virtual void draw(sf::RenderWindow &window) = 0;

    virtual bool buyHPClicked(const sf::Vector2i &mouse) = 0;
    virtual bool buyItem1Clicked(const sf::Vector2i &mouse) = 0;
    virtual bool buyItem2Clicked(const sf::Vector2i &mouse) = 0;
    virtual bool leaveClicked(const sf::Vector2i &mouse) = 0;

    virtual ~GraphicsStoreUI() = default;
};

class GraphicsCombatUI {
public:
    virtual ~GraphicsCombatUI() = default;
    virtual void update(const Player &player, const Enemy &enemy, int sp, bool isStunned) = 0;
    virtual void draw(sf::RenderWindow &window) = 0;
    virtual bool pressAttack1(const sf::Event &ev) const = 0;
    virtual bool pressAttack2(const sf::Event &ev) const = 0;
};

class EntityGraphics {
private:
    sf::RectangleShape shape_;

public:
    virtual ~EntityGraphics() = default;
    void EntityVisual() {
        shape_.setSize({150.f, 300.f});
        shape_.setFillColor(sf::Color::White);
        shape_.setOrigin(shape_.getSize() / 2.f);
    }
    void Setposition(float x, float y) {shape_.setPosition({x, y});}
    void SetColor(sf::Color c) {shape_.setFillColor(c);}

    void Draw(sf::RenderWindow &window, Entity &entity, sf::Font &font) {
        window.draw(shape_);
        sf::Text hpText(font);
        hpText.setFont(font);
        hpText.setCharacterSize(28);
        hpText.setFillColor(sf::Color::Red);
        std::ostringstream oss;
        oss <<"HP: "<< entity.getHp() << "/" << entity.getMaxHp();
        hpText.setString(oss.str());
        sf::FloatRect bounds = shape_.getGlobalBounds();
        hpText.setPosition({bounds.position.x + 10, bounds.position.y - 60});
        window.draw(hpText);
    }
};

class StoreGraphics : public GraphicsStoreUI {
private:
    sf::Font font_;
    Button btnHP_, btnItem1_, btnItem2_, btnLeave_;
    sf::RectangleShape boxHP_, boxI1_, boxI2_, boxLeave_;
    sf::Text buyTextHP_, buyText1_, buyText2_, leaveText_;
    sf::Text statHp_, statGold_, statLvl_, statXp_;
    sf::Text title_;
    sf::Text price1_, name1_, desc1_;
    sf::Text price2_, name2_, desc2_;
    sf::Text healthPrice_;

    void loadFont() {
        if (!font_.openFromFile("fonts/PlayfulTime-BLBB8.ttf")) {
            std::cerr << "Warning: could not load font: fonts/PlayfulTime-BLBB8.ttf\n";
        }
    }


public:
    StoreGraphics()
    : btnHP_(740.f, 200.f, 70.f, 60.f),
      btnItem1_(740.f, 310.f, 70.f, 60.f),
      btnItem2_(740.f, 695.f, 70.f, 60.f),
      btnLeave_(200.f, 950.f, 120.f, 60.f),
      boxHP_(),
      boxI1_(),
      boxI2_(),
      boxLeave_(),
      buyTextHP_(font_),
      buyText1_(font_),
      buyText2_(font_),
      leaveText_(font_),
      statHp_(font_),
      statGold_(font_),
      statLvl_(font_),
      statXp_(font_),
      title_(font_),
      price1_(font_),
      name1_(font_),
      desc1_(font_),
      price2_(font_),
      name2_(font_),
      desc2_(font_),
      healthPrice_(font_)
{
    loadFont();

    // Shapes
    boxHP_.setSize({btnHP_.size().x, btnHP_.size().y});
    boxHP_.setPosition(btnHP_.position());
    boxHP_.setFillColor(sf::Color(0, 250, 0));

    boxI1_.setSize({btnItem1_.size().x, btnItem1_.size().y});
    boxI1_.setPosition(btnItem1_.position());
    boxI1_.setFillColor(sf::Color(0, 250, 0));

    boxI2_.setSize({btnItem2_.size().x, btnItem2_.size().y});
    boxI2_.setPosition(btnItem2_.position());
    boxI2_.setFillColor(sf::Color(0, 250, 0));

    boxLeave_.setSize({btnLeave_.size().x, btnLeave_.size().y});
    boxLeave_.setPosition(btnLeave_.position());
    boxLeave_.setFillColor(sf::Color(0, 250, 0));

    // Title
    title_.setFont(font_);
    title_.setCharacterSize(72);
    title_.setFillColor(sf::Color::Green);
    title_.setString("SHOP");
    title_.setPosition({1250.f, 75.f});

    // Buy text
    buyTextHP_.setFont(font_);
    buyTextHP_.setCharacterSize(36);
    buyTextHP_.setFillColor(sf::Color::Black);
    buyTextHP_.setString("BUY");
    buyTextHP_.setPosition(btnHP_.position());

    buyText1_.setFont(font_);
    buyText1_.setCharacterSize(36);
    buyText1_.setFillColor(sf::Color::Black);
    buyText1_.setString("BUY");
    buyText1_.setPosition(btnItem1_.position());

    buyText2_.setFont(font_);
    buyText2_.setCharacterSize(36);
    buyText2_.setFillColor(sf::Color::Black);
    buyText2_.setString("BUY");
    buyText2_.setPosition(btnItem2_.position());

    leaveText_.setFont(font_);
    leaveText_.setCharacterSize(32);
    leaveText_.setFillColor(sf::Color::Black);
    leaveText_.setString("LEAVE");
    leaveText_.setPosition(btnLeave_.position());

    // Stats
    statHp_.setFont(font_);
    statHp_.setCharacterSize(28);
    statHp_.setFillColor(sf::Color::Red);
    statHp_.setPosition({100.f, 300.f});

    statGold_ = statHp_;
    statGold_.setPosition({100.f, 350.f});

    statLvl_ = statHp_;
    statLvl_.setPosition({100.f, 400.f});

    statXp_ = statHp_;
    statXp_.setPosition({100.f, 450.f});

    // Item 1 UI
    price1_.setFont(font_);
    price1_.setCharacterSize(30);
    price1_.setFillColor(sf::Color::Green);
    price1_.setPosition({820.f, 310.f});

    name1_.setFont(font_);
    name1_.setCharacterSize(30);
    name1_.setFillColor(sf::Color::Green);
    name1_.setPosition({740.f, 410.f});

    desc1_.setFont(font_);
    desc1_.setCharacterSize(26);
    desc1_.setFillColor(sf::Color::Green);
    desc1_.setPosition({740.f, 510.f});

    // Item 2 UI
    price2_.setFont(font_);
    price2_.setCharacterSize(30);
    price2_.setFillColor(sf::Color::Green);
    price2_.setPosition({820.f, 695.f});

    name2_.setFont(font_);
    name2_.setCharacterSize(30);
    name2_.setFillColor(sf::Color::Green);
    name2_.setPosition({740.f, 795.f});

    desc2_.setFont(font_);
    desc2_.setCharacterSize(26);
    desc2_.setFillColor(sf::Color::Green);
    desc2_.setPosition({740.f, 895.f});

    // HP restore price
    healthPrice_.setFont(font_);
    healthPrice_.setCharacterSize(30);
    healthPrice_.setFillColor(sf::Color::Green);
    healthPrice_.setPosition({820.f, 200.f});
}

    void update(const Player &player, const Items &i1, const Items &i2) override {
        std::ostringstream s;
        s << "HP: " << static_cast<int>(player.getHp()) << "/" << static_cast<int>(player.getMaxHp());
        statHp_.setString(s.str());
        statGold_.setString("Gold: " + std::to_string(player.getGold()));
        statLvl_.setString("Lvl: " + std::to_string(player.getLevel()));
        statXp_.setString("Exp: " + std::to_string(player.getExp()));

        healthPrice_.setString("Restore " + std::to_string(static_cast<int>(player.getMaxHp()/5)) + " HP (20 g)");

        price1_.setString("Price: " + std::to_string(i1.price));
        name1_.setString("Name: " + i1.name);
        desc1_.setString(i1.descripction);

        price2_.setString("Price: " + std::to_string(i2.price));
        name2_.setString("Name: " + i2.name);
        desc2_.setString(i2.descripction);

        // visual indicators for owned items
        if (i1.have) boxI1_.setFillColor({128, 0, 0});
        else boxI1_.setFillColor({0, 250, 0});

        if (i2.have) boxI2_.setFillColor({128, 0, 0});
        else boxI2_.setFillColor({0, 250, 0});
    }

    void draw(sf::RenderWindow &window) override {
        window.draw(boxHP_); window.draw(boxI1_); window.draw(boxI2_); window.draw(boxLeave_);
        window.draw(buyTextHP_); window.draw(buyText1_); window.draw(buyText2_); window.draw(leaveText_);
        window.draw(statHp_); window.draw(statGold_); window.draw(statLvl_); window.draw(statXp_);
        window.draw(healthPrice_); window.draw(title_);
        window.draw(price1_); window.draw(name1_); window.draw(desc1_);
        window.draw(price2_); window.draw(name2_); window.draw(desc2_);
    }

    bool buyHPClicked(const sf::Vector2i &mouse) override  { return btnHP_.contains(mouse); }
    bool buyItem1Clicked(const sf::Vector2i &mouse) override   { return btnItem1_.contains(mouse); }
    bool buyItem2Clicked(const sf::Vector2i &mouse) override   { return btnItem2_.contains(mouse); }
    bool leaveClicked(const sf::Vector2i &mouse) override   { return btnLeave_.contains(mouse); }


};

class CombatGraphics : GraphicsCombatUI {
private:
    sf::Font font_;
    sf::Text atk1_, atk2_, spText_, stunText_;
    EntityGraphics playerVis_, enemyVis_;
    Player playerSnapshot_;
    Enemy enemySnapshot_{10.f, 5.f, 1, 1, 1};

public:
    CombatGraphics(): atk1_(font_), atk2_(font_), spText_(font_), stunText_(font_) {
        if (!font_.openFromFile("fonts/PlayfulTime-BLBB8.ttf")) {
            std::cerr << "Warning: could not load font for combat\n";
        }

        // texts
        atk1_.setFont(font_);
        atk1_.setCharacterSize(28);
        atk1_.setFillColor(sf::Color::Red);
        atk1_.setPosition({300.f, 800.f});
        atk2_.setFont(font_);
        atk2_.setCharacterSize(28);
        atk2_.setFillColor(sf::Color::Red);
        atk2_.setPosition({300.f, 860.f});
        spText_.setFont(font_);
        spText_.setCharacterSize(28);
        spText_.setFillColor(sf::Color::Red);
        spText_.setPosition({300.f, 740.f});
        stunText_.setFont(font_);
        stunText_.setCharacterSize(28);
        stunText_.setFillColor(sf::Color::Red);
        stunText_.setPosition({1350.f, 500.f});
        // entity visuals
        playerVis_.Setposition(400.f, 500.f);
        playerVis_.SetColor(sf::Color::Blue);
        enemyVis_.Setposition(1350.f, 400.f);
        enemyVis_.SetColor(sf::Color::Red);
    }

    void update(const Player &player, const Enemy &enemy, int sp, bool isStunned) override {
        std::ostringstream s1, s2;
        s1 << "1. deals: " << static_cast<int>(player.getAtk()) << " damage";
        s2 << "2. Special attack: " << static_cast<int>(player.getAtk() * 5) << " damage (2 sp)";
        atk1_.setString(s1.str());
        atk2_.setString(s2.str());
        spText_.setString("Sp: " + std::to_string(sp));
        if (isStunned) stunText_.setString("Status: stunned");
        else stunText_.setString("");
        // you can set different colors for low hp, etc.
    }

    void draw(sf::RenderWindow &window) override {
        playerVis_.Draw(window, playerSnapshot_, font_);
        playerVis_.EntityVisual();
        playerVis_.SetColor(sf::Color::Blue);
        enemyVis_.Draw(window, enemySnapshot_, font_);
        enemyVis_.EntityVisual();
        enemyVis_.SetColor(sf::Color::Red);
        window.draw(atk1_);
        window.draw(atk2_);
        window.draw(spText_);
        window.draw(stunText_);
    }

    void setSnapshots(const Player &p, const Enemy &e) {
        playerSnapshot_ = p;
        enemySnapshot_ = e;
    }

    bool pressAttack1(const sf::Event &ev) const override {
        if (isKeyPressed(sf::Keyboard::Key::Num1)==true) return true;
        return false;
    }

    bool pressAttack2(const sf::Event &ev) const override {
        if (isKeyPressed(sf::Keyboard::Key::Num2)==true) return true;
        return false;
    }
};

class CombatLogic {
public:
    bool runCombat(Player &player, Enemy &enemy, std::vector<Items> &items, CombatGraphics &ui, sf::RenderWindow &window) {
        if (items.size() >= 2 && items[1].have && !items[1].effecton) {
            player.setMaxHp(player.getMaxHp() * 2.f);
            player.setHp(player.getHp() * 2.f);
            items[1].effecton = true;
        }
        if (items.size() >= 1 && items[0].have && !items[0].effecton) {
            player.setAtk(player.getAtk() * 1.5f);
            items[0].effecton = true;
        }

        bool isStunned = false;
        enemy.setIsDead(false);
        enemy.setHp(enemy.getMaxHp());
        int sp = player.getSP();

        while (!player.getIsDead() && !enemy.getIsDead() && window.isOpen()) {
            while (auto event = window.pollEvent()) {
                const sf::Event& e = *event;
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                    return true;
                }

                if (ui.pressAttack1(e)) {
                    int dmg = player.DoDamage();
                    enemy.TakeDamage(dmg);
                    if(enemy.getHp() < 0) enemy.setIsDead(true);
                    if (!isStunned) {
                        player.TakeDamage(enemy.getAtk());
                        if(player.getHp() < 0) player.setIsDead(true);
                    }
                    isStunned = false;
                    player.addSP(1);
                }

                if (ui.pressAttack2(e)) {
                    if (player.consumeSP(2)) {
                        int dmg = player.DoDamage(5.0f);
                        enemy.TakeDamage(dmg);
                        if(enemy.getHp() < 0) enemy.setIsDead(true);
                        isStunned = true;
                    }
                    if (!isStunned) {
                        player.TakeDamage(enemy.getAtk());
                        if(player.getHp() < 0) player.setIsDead(true);
                    }
                }
            }

            if (enemy.getIsDead()==true) break;
            if (player.getIsDead()==true) break;

            ui.setSnapshots(player, enemy);

            ui.update(player, enemy, player.getSP(), isStunned);

            window.clear(sf::Color(20,20,20));
            ui.draw(window);
            window.display();
        }

        if (enemy.getIsDead()==true) {
            int goldGain = enemy.getGold();
            int expGain = enemy.getExp();
            if (items.size() > 4 && items[4].have) goldGain *= 2;
            if (items.size() > 5 && items[5].have) expGain *= 2;
            player.setGold(player.getGold() + goldGain);
            player.setExp(player.getExp() + expGain);
            player.tryLvlUp();
            if (items.size() > 3 && items[3].have && !items[3].effecton) {
                int healAmount = static_cast<int>(enemy.getHp() / 10.f);
                player.setHp(player.getHp() + healAmount);
                items[3].effecton = true;
            }
        }
        player.addSP(-player.getSP());

        return player.getIsDead();
    }
};

class StoreLogic {
public:
    void healPlayer(Player &player) {
        const int healCost = 20;
        if (player.getGold() < healCost) return;
        int amount = static_cast<int>(player.getMaxHp() / 5);
        player.setHp(std::min(player.getHp() + amount, player.getMaxHp()));
        player.setGold(player.getGold() - healCost);
    }

    void buyItem(Player &player, Items &item) {
        if (item.have) return;
        if (player.getGold() < item.price) return;
        item.have = true;
        player.setGold(player.getGold() - item.price);
    }
};

class StoreController {
private:
    GraphicsStoreUI &ui_;
    StoreLogic &logic_;
public:
    StoreController(GraphicsStoreUI &ui, StoreLogic &logic)
        : ui_(ui), logic_(logic) {}

    void open(Player &player, sf::RenderWindow &window, std::vector<Items> &items) {
        if (items.size() < 2) {
            std::cerr << "Need at least 2 items for the shop\n";
            return;
        }
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        size_t r1 = std::rand() % (items.size() - 1);
        size_t r2 = std::rand() % (items.size() - 1);
        while (r1 == r2) r2 = std::rand() % (items.size() - 1);
        bool running = true;

        while (running && window.isOpen()) {
            while (auto event = window.pollEvent()) {
                if (const auto* keyPressed = event->getIf<sf::Event::Closed>()) {
                    window.close();
                    return;
                }
                if (const auto* keyPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    if (ui_.buyHPClicked(mouse)) {
                        logic_.healPlayer(player);
                    } else if (ui_.buyItem1Clicked(mouse)) {
                        logic_.buyItem(player, items[r1]);
                    } else if (ui_.buyItem2Clicked(mouse)) {
                        logic_.buyItem(player, items[r2]);
                    } else if (ui_.leaveClicked(mouse)) {
                        running = false;
                    }
                }
            }

            ui_.update(player, items[r1], items[r2]);
            window.clear(sf::Color(30,30,30));
            ui_.draw(window);
            window.display();
        }
    }
};







