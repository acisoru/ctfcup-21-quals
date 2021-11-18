<?php

use \Psr\Http\Message\ServerRequestInterface as Request;
use \Psr\Http\Message\ResponseInterface as Response;
use Slim\Factory\AppFactory;
use Slim\Views\Twig;
use Slim\Views\TwigMiddleware;


require __DIR__ . '/../vendor/autoload.php';
require_once "core.php";

session_start();

// Create Container
$app = AppFactory::create();


$twig = Twig::create('./templates', ['cache' => false]);

// Add Twig-View Middleware
$app->add(TwigMiddleware::create($app, $twig));

$app->addErrorMiddleware(false, true, true);


$app->get('/', function (Request $request, Response $response, $args) {
    $view = Twig::fromRequest($request);
    return $view->render($response, 'index.html', []);
});

$app->get('/register', function (Request $request, Response $response, $args) {
    $view = Twig::fromRequest($request);
    return $view->render($response, 'register.html', []);
});

$app->post('/register', function (Request $request, Response $response, $args) {
    $body = $request->getParsedBody();

    $username = $body['username'] ?? "";
    $password = $body['password'] ?? "";

    $db = getDB();

    $userID = registerUser($db, $username, $password);
    $_SESSION['user_id'] = $userID;
    $_SESSION['username'] = $username;
    $_SESSION['token'] = sha1($username . $password . $userID);

    return $response->withStatus(302)->withHeader('Location', '/home');
});

$app->get('/login', function (Request $request, Response $response, $args) {
    $view = Twig::fromRequest($request);
    return $view->render($response, 'login.html', []);
});


$app->post('/login', function (Request $request, Response $response, $args) {
    $body = $request->getParsedBody();

    $username = $body['username'] ?? "";
    $password = $body['password'] ?? "";

    $db = getDB();

    $res = loginUser($db, $username, $password);
    if (empty($res)) {
        $response = $response->withStatus(401);
        $response->getBody()->write('No such user!');
        return $response;
    }
    $userID = $res['id'];
    $_SESSION['user_id'] = $userID;
    $_SESSION['username'] = $username;
    $_SESSION['token'] = md5($username . $password . $userID);

    return $response->withStatus(302)->withHeader('Location', '/home');

});

$app->get('/home', function (Request $request, Response $response, $args) {
    if (!isset($_SESSION['user_id'])) {
        $response->withStatus(401)->getBody()->write('Unauthenticated');
        return $response;
    }
    $view = Twig::fromRequest($request);
    return $view->render($response, 'home.html', [
        "u_id" => $_SESSION['user_id'],
        'username' => $_SESSION['username'],
        'tok' => getToken(getDB(), $_SESSION['user_id']),
    ]);
});

$app->get('/stats', function (Request $request, Response $response, $args) {
    if (!isset($_SESSION['user_id'])) {
        $response->withStatus(401)->getBody()->write('Unauthenticated');
    }
    $ch = getCH();
    $platformStats = mostPlatforms($ch, $_SESSION['user_id']);
    $ipStats = mostIPs($ch, $_SESSION['user_id']);
    $qParams = $request->getQueryParams();
    $hottest = hottestQueries($ch, $_SESSION['user_id'], $qParams["ip"] ?? null, $qParams["platform"] ?? null);
    $view = Twig::fromRequest($request);
    return $view->render($response, 'stats.html', [
        "platformstats" => $platformStats,
        "ipstats" => $ipStats,
        "top_q" => $hottest,
        "username" => $_SESSION['username'],
    ]);
});

$app->post('/document', function (Request $request, Response $response, $args) {
    if (!isset($_SESSION['user_id'])) {
        $response->withStatus(401)->getBody()->write('Unauthenticated');
        return $response;
    }
    $userID = $_SESSION['user_id'];
    $body = $request->getParsedBody();
    $text = $body['text'] ?? "";
    $title = $body['title'] ?? "";

    addDocument($userID, $title, $text);

    return $response->withStatus(302)->withHeader('Location', '/home');
})->setName('document');

$app->get('/search/{index}', function (Request $request, Response $response, array $args) {
    $index = $args['index'];
    $wantToken = getToken(getDB(), $index);

    $qParams = $request->getQueryParams();

    if ($qParams['token'] !== $wantToken) {
        $response->withStatus(401)->getBody()->write('Invalid token');
        return $response;
    }

    $q = $request->getQueryParams()['q'] ?? "";
    $matches = [];
    if (!empty($q)) {
        $ms = getMS();
        $msIndex = $ms->index("index_${index}");
        $searchRes = $msIndex->search($q);
        foreach ($searchRes->getHits() as $hit) {
            $matches[] = $hit;
        }

        $ua = ($request->getHeaders()['User-Agent'] ?? [])[0] ?? "";
        $platform = getPlatform($ua);
        $xff = ($request->getHeaders()['X-Forwarded-For'] ?? [])[0] ?? null;
        logQuery($q, $xff, $platform, $index);
    }

    $view = Twig::fromRequest($request);
    return $view->render($response, 'search.html', [
        'matches' => $matches,
        'token' => $qParams['token'],
    ]);

})->setName('search');


$app->run();