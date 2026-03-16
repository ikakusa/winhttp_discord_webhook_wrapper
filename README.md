# A basic Discord Webhook wrapper using winhttp
requires C++17

requires [nlohmann::json](https://github.com/nlohmann/json)

# Example

```cpp
int main()
{
    //Setup
    Discord discord = Discord("/api/webhooks/...");
    
    //Example Embed
    Embed emb;
    emb.setTitle("this is my title!")
       .setDescription("this is my description!")
       .addField("Field 1", "1", false);
    
    //Add Multiple Embeds
    EmbedBuilder builder;
    builder.addEmbed(emb);
    builder.addEmbed(emb);

    //Message::embed function requires Embed class
    //Message::addEmbeds function requires EmbedBuilder class
    if (discord.sendWebhook(Message("This is my:\nMessage content!").addEmbeds(builder).embed(Embed().setTitle("this is my title!").setColor("#ffffff")))) {
        printf("sent to webhook!!!!!!");
    };
}
```

# Preview
![image.png](./image_preview.png)
