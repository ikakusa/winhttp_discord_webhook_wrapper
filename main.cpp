#include <iostream>
#include "Discord.h"

int main()
{
    //Setup
    Discord discord = Discord("/api/webhooks/1482728611827159114/w710-jvUYx6mrh6hOJpXLHAXelVoUabrr6hv0i_XMgEQoxr15DaGbogv9EO25lfTQswQ");
    
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
    embed3.setTitle("This is my second!")
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
    if (discord.sendWebhook(Message("This is my:\nMessage content!").addEmbeds(builder).embed(Embed().setTitle("this is end").setColor("#ffffff")))) {
        printf("sent to webhook!!!!!!");
    };
}