run:
	docker-compose build server
	docker-compose up server

deploy:
	docker-compose down
	make run

test:
	docker-compose build test
	docker-compose up test