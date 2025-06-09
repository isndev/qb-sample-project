
#include <http/http.h>
#include <qb/actor.h>
#include <qb/main.h>

// simple http actor
class HttpActor : public qb::Actor
                , public qb::http::Server<> {
    public:
    bool onInit() override {
        router().get("/", [](auto ctx) {
            ctx->response().set_content_type("application/json");
            ctx->response().body() = qb::json{
                {"status", "ok"},
                {"date", qb::http::date::now()}
            };
            ctx->complete();
        });

        router().compile();
        if (!listen({"tcp://0.0.0.0:8080"}))
            return false;

        start();
        return true;
    }
};

int main() {
    qb::Main main;

    main.core(0)
        .addActor<HttpActor>();

    std::cout << "Server started listening on http://localhost:8080, Ctrl-C to quit" << std::endl;
    main.start();
    main.join();
    return 0;
}
