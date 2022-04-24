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
        // We have 5 + 1 systems:
        // source, drain, food, air, LED, and the "root" system: id, delay_on, delay_off, pulse_width
        return {
           data: {
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
                    "delay_on": 3,
                    "delay_off": 2,
                    "pulse_width": 1
                }
            }
        }
    },
    beforeMount() {
        // This is a lifecycle function. It is called everytime the component "mounts"
        // https://vuejs.org/guide/essentials/lifecycle.html#lifecycle-diagram
        // Same old http request as in window.onload except...
        axios.get(`http://192.168.50.36:5000/device/${this.data.id}`).then(response => {
            console.log(this.data)
            this.data = response.data;
        });
    },
    methods: {
        submit(e) {
            //Use our saved data object and send it to the API (We don't use any DOM access here)
            axios.post(`http://192.168.50.36:5000/device/${this.data.id}`, this.data).then(response => {
                // validate response
            });
        },
        update(event) {
            // event: { system_name: string, field: ['pulse_width'|'delay_on'|'delay_off'], value: Number }
            if(value.system_name === "root") {
                this.data[event.field] = event.value;
            } else {
                // If it's not the root, then add the system
                this.data[event.system_name][event.field] = event.value;
            }
        }
    },
    // template is the HTML part of the component. @submit.prevent="submit" does just what you think, prevents default submit. @click is a vue wrapper for onclick and submit is the name of the function being called on click
    // prop (input/param) syntax for child component(s)
    // :prop-name="variable"

    // event syntax
    // @event:name="handler"
    template: `
    <form @submit.prevent="submit">
        <system-component system_name="root" 
            :delay_on="this.data.delay_on"
            :delay_off="this.data.delay_off" 
            :pulse_width="this.data.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="source" 
            :delay_on="this.data.source.delay_on"
            :delay_off="this.data.source.delay_off" 
            :pulse_width="this.data.source.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="drain" 
            :delay_on="this.data.drain.delay_on"
            :delay_off="this.data.drain.delay_off" 
            :pulse_width="this.data.drain.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name='food' 
            :delay_on="this.data.food.delay_on"
            :delay_off="this.data.food.delay_off" 
            :pulse_width="this.data.food.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="air" 
            :delay_on="this.data.air.delay_on"
            :delay_off="this.data.air.delay_off" 
            :pulse_width="this.data.air.pulse_width"
            @update="update">
        </system-component>
        <system-component system_name="LED" 
            :delay_on="this.data.LED.delay_on"
            :delay_off="this.data.LED.delay_off" 
            :pulse_width="this.data.LED.pulse_width"
            @update="update">
        </system-component>
        <button @click="submit" class="btn btn-primary">Update</button>
    </form>
    `
}