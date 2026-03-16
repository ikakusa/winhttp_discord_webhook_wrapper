# A basic Discord Webhook wrapper using winhttp
requires C++17

requires [nlohmann::json](https://github.com/nlohmann/json)

# Example

```cpp
#include <iostream>
#include "Discord.h"

int main()
{
    //Setup
    Discord discord = Discord("https://discord.com/api/webhooks/1483076544896176340/80Mxr-R02Z8SxdP19A2dkisLhijqkKuqTUv3uaN568tBYaR1NrHg34IDG4LtsUu9wFar");
    discord.username.set("ika286");
    //discord.avatar.set("URL"); <---- change avatar url

    //Example Embed
    Embed embed1;
    embed1.setTitle("This is my title!")
        .setDescription("this is my description!")
        .setColor("#ffffff");

    //name, value, inline
    Embed embed2;
    embed2.setTitle("This is my second!")
          .setDescription("this is my description!")
          .setColor("#ffffff")
          .addField("Field1", "My Value", true)
          .addField("Field2", "My Value2", true)
          .addField({
              { "name", "Field3" },
              { "value", "My Value3" },
              { "inline", true }
          });

    Embed embed3;
    embed3.setTitle("This is my third!")
          .setDescription("this is my description!")
          .setColor("#ffffff")
          .setFooter("ikakusa", "https://avatars.githubusercontent.com/u/182407710?v=4&size=64");
    
    //Add Multiple Embeds
    EmbedBuilder builder;
    builder.addEmbed(embed1);
    builder.addEmbed(embed2);
    builder.addEmbed(embed3);

    //Message::embed function requires Embed class
    //Message::addEmbeds function requires EmbedBuilder class
    Message message("This is my:\nMessage content!");
    message.addEmbeds(builder)
           .embed(Embed().setTitle("this is end").setColor("#ffffff"));

    if (discord.sendWebhook(message, DiscordFormData())) {
        printf("sent to webhook!!!!!!");
    };
}
```

# Preview
![image.png](./image_preview.png)

# TODO

- [ ] Timestamp
- [ ] FormData::add_image()
