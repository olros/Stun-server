run:
	docker-compose build
	docker-compose up

deploy:
	docker-compose down
	make run

test:
	docker-compose build
	docker-compose run udp "./tests/runTests"