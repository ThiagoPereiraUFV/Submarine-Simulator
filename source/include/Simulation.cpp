#include "Simulation.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

Rng makeSystemRng() {
	auto engine = std::make_shared<std::mt19937>(std::random_device{}());
	return [engine](int n) { return std::uniform_int_distribution<int>(0, n - 1)(*engine); };
}

Simulation::Simulation(Object3D submarine, Object3D fish, Object3D shark, Object3D ship,
					   Object3D helicopter, Rng rng)
	: submarine_(std::move(submarine)), fishTemplate_(std::move(fish)),
	  sharkTemplate_(std::move(shark)), shipTemplate_(std::move(ship)),
	  helicopterTemplate_(std::move(helicopter)), rng_(std::move(rng)) {
	submarine_.setColor({30.0 / 255.0, 50.0 / 255.0, 20.0 / 255.0});
}

double Simulation::randomSignedCoordinate(int range) {
	// rand()%2==0 decides the sign, rand()%range the magnitude -- always
	// consume both, in that order, so seeded runs stay reproducible
	// regardless of which sign is picked.
	bool negative = rng_(2) == 0;
	int magnitude = rng_(range);
	return negative ? -magnitude : magnitude;
}

Vec3 Simulation::randomColor() {
	double r = rng_(255) / 255.0;
	double g = rng_(255) / 255.0;
	double b = rng_(255) / 255.0;
	return {r, g, b};
}

void Simulation::spawn(int spawnRange) {
	for (int i = 0; i < 20; ++i) {
		Object3D fish = fishTemplate_;
		double x = randomSignedCoordinate(spawnRange);
		double z = randomSignedCoordinate(spawnRange);
		double y = -static_cast<double>(rng_(spawnRange));
		fish.setPosition({x, y, z});
		fish.setRotY(rng_(360));
		fish.setColor(randomColor());
		fishes_.push_back(fish);

		Object3D shark = sharkTemplate_;
		double sx = randomSignedCoordinate(spawnRange);
		double sz = randomSignedCoordinate(spawnRange);
		double sy = -static_cast<double>(rng_(spawnRange)) - 5.0;
		shark.setPosition({sx, sy, sz});
		shark.setRotY(rng_(360));
		shark.setColor(randomColor());
		sharks_.push_back(shark);
	}

	const int shipHeliRange = 1000;
	for (int i = 0; i < 5; ++i) {
		Object3D ship = shipTemplate_;
		double x = randomSignedCoordinate(shipHeliRange);
		double z = randomSignedCoordinate(shipHeliRange);
		ship.setPosition({x, 0.0, z});
		ship.setRotY(rng_(360));
		double gray = rng_(255) / 255.0;
		ship.setColor({gray, gray, gray});
		ships_.push_back(ship);
	}

	for (int i = 0; i < 5; ++i) {
		Object3D heli = helicopterTemplate_;
		double x = randomSignedCoordinate(shipHeliRange);
		double z = randomSignedCoordinate(shipHeliRange);
		double y = rng_(shipHeliRange / 5) + 50;
		heli.setPosition({x, y, z});
		helicopters_.push_back(heli);
	}
}

void Simulation::setViewport(int width, int height) {
	(void)width; // kept for interface symmetry with the renderer; only height drives the sea cube
	viewHeight_ = height;
	const double h = height;
	seaCorners_ = {{-h, -h, h},	 {-h, h, h},  {h, h, h},  {h, -h, h},
				   {-h, -h, -h}, {-h, h, -h}, {h, h, -h}, {h, -h, -h}};

	if (!started_) {
		viewer_ = {0.0, 2.5 * 0.003 * h + 10, 5.0 * 0.003 * h + 10};
		center_ = submarine_.position();
		spawn(height);
		started_ = true;
	}
}

void Simulation::stepSubmarine() {
	const double radRotation = submarine_.rotY() * (M_PI / 180.0);
	const double cosRot = std::cos(radRotation), sinRot = std::sin(radRotation);
	const float maxVelocity = 5.0f, maxAcceleration = 0.2f, deltaAcc = 0.05f, thrustAcc = 0.1f;

	if (controls_.accelerate) {
		velocity_ += controls_.deltaVel;
		controls_.deltaVel += (controls_.deltaVel > 0.0f) ? deltaAcc : -deltaAcc;
		velocity_ = (std::abs(velocity_) >= maxVelocity)
						? (velocity_ / std::abs(velocity_)) * maxVelocity
						: velocity_;
		controls_.deltaVel =
			(std::abs(controls_.deltaVel) >= maxAcceleration)
				? (controls_.deltaVel / std::abs(controls_.deltaVel)) * maxAcceleration
				: controls_.deltaVel;
	} else {
		velocity_ = (std::abs(velocity_) <= thrustAcc) ? 0.0f
					: (velocity_ > 0.0f)			   ? velocity_ - thrustAcc
													   : velocity_ + thrustAcc;
	}

	viewer_.z -= velocity_ * cosRot;
	center_.z -= velocity_ * cosRot;
	viewer_.x += velocity_ * sinRot;
	center_.x += velocity_ * sinRot;

	const double degRotation = submarine_.rotY();
	const float maxAngVel = 1.0f;

	if (controls_.turn) {
		angularVelocity_ += controls_.deltaAngVel;
		angularVelocity_ = (std::abs(angularVelocity_) >= maxAngVel)
							   ? (angularVelocity_ / std::abs(angularVelocity_)) * maxAngVel
							   : angularVelocity_;
		controls_.deltaAngVel =
			(std::abs(controls_.deltaAngVel) >= maxAngVel)
				? (controls_.deltaAngVel / std::abs(controls_.deltaAngVel)) * maxAngVel
				: controls_.deltaAngVel;
	} else {
		angularVelocity_ = (std::abs(angularVelocity_) <= thrustAcc) ? 0.0f
						   : (angularVelocity_ > 0.0f)				 ? angularVelocity_ - thrustAcc
																	 : angularVelocity_ + thrustAcc;
	}

	if (controls_.emerge) {
		if (center_.y < 0.0) {
			viewer_.y += 1.0;
			center_.y += 1.0;
		}
	} else if (controls_.immerse) {
		if (center_.y >= -static_cast<double>(viewHeight_)) {
			viewer_.y -= 1.0;
			center_.y -= 1.0;
		}
	}

	const double deltaY = 0.01;
	if (!bobbingUp_) {
		if (bobbingStep_ >= 30) {
			bobbingUp_ = true;
		} else {
			bobbingStep_++;
			center_.y += deltaY;
		}
	} else {
		if (bobbingStep_ <= 0) {
			bobbingUp_ = false;
		} else {
			bobbingStep_--;
			center_.y -= deltaY;
		}
	}

	submarine_.setPosition(center_);
	submarine_.setRotY(degRotation + angularVelocity_);
	submarine_.setRotZ(angularVelocity_);
}

void Simulation::stepSeaAnimals() {
	const std::size_t n = std::min(fishes_.size(), sharks_.size());
	for (std::size_t i = 0; i < n; ++i) {
		const double radF = fishes_[i].rotY() * (M_PI / 180.0);
		Vec3 fp = fishes_[i].position();
		fishes_[i].setPosition({fp.x + 0.1 * std::sin(-radF), fp.y, fp.z + 0.1 * std::cos(-radF)});

		const double radS = sharks_[i].rotY() * (M_PI / 180.0);
		Vec3 sp = sharks_[i].position();
		sharks_[i].setPosition(
			{sp.x + 0.1 * std::sin(4.71239 - radS), sp.y, sp.z + 0.1 * std::cos(4.71239 - radS)});
	}
}

void Simulation::stepShips() {
	for (Object3D& ship : ships_) {
		const double rad = ship.rotY() * (M_PI / 180.0);
		Vec3 p = ship.position();
		ship.setPosition(
			{p.x + 0.1 * std::sin(4.71239 - rad), p.y, p.z + 0.1 * std::cos(4.71239 - rad)});
	}
}

void Simulation::pressSpecial(SpecialKey key) {
	switch (key) {
	case SpecialKey::Up:
		controls_.emerge = true;
		break;
	case SpecialKey::Down:
		controls_.immerse = true;
		break;
	case SpecialKey::Right:
		controls_.deltaAngVel = 0.1f;
		controls_.turn = true;
		break;
	case SpecialKey::Left:
		controls_.deltaAngVel = -0.1f;
		controls_.turn = true;
		break;
	case SpecialKey::F11:
	case SpecialKey::Other:
		break;
	}
}

void Simulation::releaseSpecial(SpecialKey key) {
	switch (key) {
	case SpecialKey::Up:
		controls_.emerge = false;
		break;
	case SpecialKey::Down:
		controls_.immerse = false;
		break;
	case SpecialKey::Right:
	case SpecialKey::Left:
		controls_.deltaAngVel = 0.0f;
		controls_.turn = false;
		break;
	case SpecialKey::F11:
	case SpecialKey::Other:
		break;
	}
}

bool Simulation::pressKey(unsigned char key) {
	switch (key) {
	case 'W':
	case 'w':
		controls_.accelerate = true;
		controls_.deltaVel = 0.01f;
		break;
	case 'S':
	case 's':
		controls_.accelerate = true;
		controls_.deltaVel = -0.01f;
		break;
	case 'H':
	case 'h':
		help_ = !help_;
		break;
	case 'F':
	case 'f':
		firstPerson_ = false;
		break;
	case 'I':
	case 'i':
		firstPerson_ = true;
		break;
	case 'L':
	case 'l':
		if (lighting_) {
			lighting_ = false;
			light0On_ = light1On_ = true;
		} else {
			lighting_ = true;
		}
		break;
	case 'G':
	case 'g':
		smoothShading_ = !smoothShading_;
		break;
	case '1':
		if (light0On_ && lighting_)
			light0On_ = false;
		else if (lighting_)
			light0On_ = true;
		break;
	case '2':
		if (light1On_ && lighting_)
			light1On_ = false;
		else if (lighting_)
			light1On_ = true;
		break;
	case 27:
		return true;
	}
	return false;
}

void Simulation::releaseKey(unsigned char key) {
	switch (key) {
	case 'W':
	case 'w':
	case 'S':
	case 's':
		controls_.accelerate = false;
		controls_.deltaVel = 0.0f;
		break;
	}
}

Vec3 Simulation::backgroundColor() const {
	if (lighting_ && !light0On_)
		return {8.0 / 255.0, 24.0 / 255.0, 97.0 / 255.0};
	return {0.0, 227.0 / 255.0, 1.0};
}

double Simulation::sunRadius() const {
	return (lighting_ && !light0On_) ? 0.1 : 0.2;
}

std::vector<HelpLine> Simulation::helpLines() const {
	return {
		{" W para acelerar", 0.0},		  {" S para re", -1.0},
		{" Cima para emergir", -2.0},	  {" Baixo para imergir", -3.0},
		{" Esquerda para virar", -4.0},	  {" Direita para virar", -5.0},
		{" F ponto de vista fora", -6.0}, {" I ponto de vista dentro", -7.0},
	};
}
