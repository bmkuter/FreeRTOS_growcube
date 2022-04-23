// This file is the entirety of the app Component
// Vue components have data and lifecycle management functions
// Interact with the components only through lifecycle functions, never through the DOM e.g. getElementById and element.addClass("myClass")
import SystemComponent from "./system.js"

export default {
    components: {
        SystemComponent
    },
    data() {
        // This is the state of the component. 
        return {
            settings: {
                "id": 2,
                "delay_on": 0,
                "delay_off": 0,
                "pulse_width": 0,
                "source": {
                    "delay_on": 0,
                    "delay_off": 0,
                    "pulse_width": 0
                },
                "drain": {
                    "delay_on": 0,
                    "delay_off": 0,
                    "pulse_width": 0
                },
                "food": {
                    "delay_on": 0,
                    "delay_off": 0,
                    "pulse_width": 0
                },
                "air": {
                    "delay_on": 0,
                    "delay_off": 0,
                    "pulse_width": 0
                },
                "LED": {
                    "delay_on": 0,
                    "delay_off": 0,
                    "pulse_width": 0
                }
            }
        }
    },
    mounted() {
        // This is a alifecycle function. It is called everytime the component "mounts"
        // A mount == reload, essentially
        // Same old http request as in window.onload except...
        axios.get(`http://192.168.50.36:5000/device/${this.settings.id}`).then(response => {
            this.settings = response.data;
            console.log(this.settings)
        });
    },
    methods: {
        submit(e) {
            //Use our saved data object and send it to the API (We don't use any DOM access here)
            console.log("HTTP REQUEST GOES HERE");
            axios.post(`http://192.168.50.36:5000/device/${this.settings.id}`, this.settings).then(response => {

                console.log(response);
            });
        },
        update(value) {
            console.log(value);
            if(value.system_name === "root") {
                this.settings.delay_off = value.delay_off
                this.settings.delay_on = value.delay_on
                this.settings.pulse_width = value.pulse_width
            } else {
                this.settings[value.system_name].delay_off = value.delay_off
                this.settings[value.system_name].delay_on = value.delay_on
                this.settings[value.system_name].pulse_width = value.pulse_width
            }
        }
    },
    // template is the HTML part of the component. @submit.prevent="submit" does just what you think, prevents default submit. @click is a vue wrapper for onclick and submit is the name of the function being called on click
    template: `
    <form @submit.prevent="submit">
        <system-component system_name="root" 
            :delay_on="this.settings.delay_on"
            :delay_off="this.settings.delay_off" 
            :pulse_width="this.settings.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="source" 
            :delay_on="this.settings.source.delay_on"
            :delay_off="this.settings.source.delay_off" 
            :pulse_width="this.settings.source.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="drain" 
            :delay_on="this.settings.drain.delay_on"
            :delay_off="this.settings.drain.delay_off" 
            :pulse_width="this.settings.drain.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name='food' 
            :delay_on="this.settings.food.delay_on"
            :delay_off="this.settings.food.delay_off" 
            :pulse_width="this.settings.food.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="air" 
            :delay_on="this.settings.air.delay_on"
            :delay_off="this.settings.air.delay_off" 
            :pulse_width="this.settings.air.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="LED" 
            :delay_on="this.settings.LED.delay_on"
            :delay_off="this.settings.LED.delay_off" 
            :pulse_width="this.settings.LED.pulse_width"
            @update="update">
        </system-component>
        <button @click="submit" class="btn btn-primary">Update</button>
    </form>
    `
}