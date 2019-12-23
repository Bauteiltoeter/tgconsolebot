# Telegram console bot

This program creates is telegram interface to use from the linux command line or in scripts.

You can configure multiple telegram bot accounts to be used by this bot.
The bot can send messages, stickers and files to previously configured useres.

This program uses sessions to provide support for multiple bots on one machine.
Each session is connected to a different telegram bot. Each session has its own list of authorized users.

## Usage

### Send stuff

You can send a text message to all configured users in the default session like this:
<pre>
./tgconsolebot -m 'Hello world'
./tgconsolebot --message 'Hello world'
./tgconsolebot -s ok-sticker
./tgconsolebot -f path/to/file.txt
</pre>

### Create a bot

Create a telegram bot using the @BotFather bot.
The BotFather will send you an access token.

### Setup a tgconsolebot session

The tgconsolebot session is specified with the --session option. If you do not specify a session, the session "default" is used.

You can set the access token for the current session with the command --set-token.

The token is stored in a configuration file (see section below).

### Add users to a session

During normal operation, the bot does not accept new users. If you want to add new users to a session, start the bot with the --add-user switch.

In this mode, the bot will stay active and listens for telegram /start commands. Start the bot with --add-user, search for your bot in telegram and write /start.

The bot will notify use as soon as a new user has joind the session
<pre>
Got new user, id:122829708
Created user Karrn; id: 122829708
</pre>

### Add names to stickers

Normaly, stickers are identified by unique IDs. These IDs are hard to remember and do not have a telling name. For better readability, this bot provides the posibilit to give telling name to stickers.

To configure stickers, start the bot with the --add-sticker option.

The bot will start to listen for new messages. Just send the new sticker to your bot and the command line will come up with an input line to specify the new sticker name.

<pre>
Bot username: DragonStorageBot
Got message with sticker id: CAADBAAD-wAEvrEHDZK6DiawRJsWBA
Please enter name for this sticker: foodsticker

New name:foodsticker
</pre>

The unique id and the name are stored in a configuration file. From this point on, you can refere to the sticker by its new name.

<pre>
./tgconsolebot -s foodsticker
</pre>

## Build the tgconsolebot

### TgBot

This program uses telegram C++ bindings made by reo7sp
https://github.com/reo7sp/tgbot-cpp

Clone and install the library according to the github documentation.

### Boost program options

This program needs libboost-program-options-dev to compile.

### Compile

<pre>
cmake .
make -j8
</pre>
